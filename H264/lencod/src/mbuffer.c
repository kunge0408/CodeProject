
/*!
 ***********************************************************************
 *  \file
 *      mbuffer.c
 *
 *  \brief
 *      Frame buffer functions
 *
 *  \author
 *      Main contributors (see contributors.h for copyright, address and affiliation details)
 *      - Karsten S�hring          <suehring@hhi.de>
 ***********************************************************************
 */

#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include <limits.h>

#include "global.h"
#include "mbuffer.h"
#include "memalloc.h"
#include "output.h"
#include "image.h"
#define RSD(x) ((x&2)?(x|1):(x&(~1)))

//-------- start new Buffer code -----------------------------
static void insert_picture_in_dpb(FrameStore* fs, StorablePicture* p);
static void output_one_frame_from_dpb();
static int  is_used_for_reference(FrameStore* fs);
static void get_smallest_poc(int *poc,int * pos);
static int  remove_unused_frame_from_dpb();
static int  is_short_term_reference(FrameStore* fs);
static int  is_long_term_reference(FrameStore* fs);

DecodedPictureBuffer dpb;

StorablePicture **listX[6];

int listXsize[6];

/*!
 ************************************************************************
 * \brief
 *    Print out list of pictures in DPB. Used for debug purposes.
 ************************************************************************
 */
void dump_dpb()
{
  unsigned i;

  return;
  
  for (i=0; i<dpb.used_size;i++)
  {
    printf("(");
    printf("fn=%d  ", dpb.fs[i]->frame_num);
    if (dpb.fs[i]->is_used & 1)
      printf("T: poc=%d  ", dpb.fs[i]->top_field->poc);
    if (dpb.fs[i]->is_used & 2)
      printf("B: poc=%d  ", dpb.fs[i]->bottom_field->poc);
    if (dpb.fs[i]->is_used == 3)
      printf("F: poc=%d  ", dpb.fs[i]->frame->poc);
    printf("G: poc=%d)  ", dpb.fs[i]->poc);
    if (dpb.fs[i]->is_reference) printf ("ref  ");
    if (dpb.fs[i]->is_output) printf ("out  ");
    printf ("\n");
  }
}


/*!
 ************************************************************************
 * \brief
 *    Allocate memory for decoded picture buffer an initialize with sane values.
 *
 * \param inp
 *    Input Parameters
 *
 ************************************************************************
 */
void init_dpb(InputParameters *inp)
{
  unsigned i,j;

	dpb.size      = inp->num_reference_frames;

  dpb.used_size = 0;

  dpb.fs = calloc(dpb.size, sizeof (FrameStore*));
  if (NULL==dpb.fs) 
    no_mem_exit("init_dpb: dpb->fs");

  dpb.fs_ref = calloc(dpb.size, sizeof (FrameStore*));
  if (NULL==dpb.fs_ref) 
    no_mem_exit("init_dpb: dpb->fs_ref");

  dpb.fs_ltref = calloc(dpb.size, sizeof (FrameStore*));
  if (NULL==dpb.fs_ltref) 
    no_mem_exit("init_dpb: dpb->fs_ltref");

  for (i=0; i<dpb.size; i++)
  {
    dpb.fs[i]       = alloc_frame_store();
    dpb.fs_ref[i]   = NULL;
    dpb.fs_ltref[i] = NULL;
  }
  
  for (i=0; i<6; i++)
  {
    listX[i] = calloc(dpb.size * 2 + 1, sizeof (StorablePicture*)); // +1 for reordering
    if (NULL==listX[i]) 
      no_mem_exit("init_dpb: listX[i]");
  }

  for (j=0;j<6;j++)
  {
    for (i=0; i<(2*dpb.size)+1; i++)
    {
      listX[j][i] = NULL;
    }
    listXsize[j]=0;
  }

  dpb.last_output_poc = INT_MIN;
}


/*!
 ************************************************************************
 * \brief
 *    Free memory for decoded picture buffer.
 ************************************************************************
 */
void free_dpb()
{
  unsigned i;
  if (dpb.fs)
  {
    for (i=0; i<dpb.size; i++)
    {
      free_frame_store(dpb.fs[i]);
    }
    free (dpb.fs);
    dpb.fs=NULL;
  }
  if (dpb.fs_ref)
  {
    free (dpb.fs_ref);
  }
  if (dpb.fs_ltref)
  {
    free (dpb.fs_ltref);
  }
  dpb.last_output_poc = INT_MIN;

  for (i=0; i<6; i++)
    if (listX[i])
    {
      free (listX[i]);
      listX[i] = NULL;
    }
}


/*!
 ************************************************************************
 * \brief
 *    Allocate memory for decoded picture buffer frame stores an initialize with sane values.
 *
 * \return
 *    the allocated FrameStore structure
 ************************************************************************
 */
FrameStore* alloc_frame_store()
{
  FrameStore *f;

  f = calloc (1, sizeof(FrameStore));
  if (NULL==f) 
    no_mem_exit("alloc_frame_store: f");

  f->is_used      = 0;
  f->is_reference = 0;

  f->is_output = 0;

  f->frame        = NULL;;
  f->top_field    = NULL;
  f->bottom_field = NULL;

  return f;
}

/*!
 ************************************************************************
 * \brief
 *    Allocate memory for a stored picture. 
 *
 * \param size_x
 *    horizontal luma size
 * \param size_y
 *    vertical luma size
 * \param size_x_cr
 *    horizontal chroma size
 * \param size_y_cr
 *    vertical chroma size
 *
 * \return
 *    the allocated StorablePicture structure
 ************************************************************************
 */
StorablePicture* alloc_storable_picture(PictureStructure structure, int size_x, int size_y, int size_x_cr, int size_y_cr)
{
  StorablePicture *s;

  //printf ("Allocating (%s) picture (x=%d, y=%d, x_cr=%d, y_cr=%d)\n", (type == FRAME)?"FRAME":(type == TOP_FIELD)?"TOP_FIELD":"BOTTOM_FIELD", size_x, size_y, size_x_cr, size_y_cr);

  s = calloc (1, sizeof(StorablePicture));
  if (NULL==s) 
    no_mem_exit("alloc_storable_picture: s");

  get_mem2D (&(s->imgY), size_y, size_x);
  
  s->imgY_11 = NULL;
  s->imgY_ups = NULL;

  get_mem3D (&(s->imgUV), 2, size_y_cr, size_x_cr );

  s->mb_field = calloc (img->PicSizeInMbs, sizeof(int));

  get_mem3Dint (&(s->ref_idx), 2, size_x / BLOCK_SIZE, size_y / BLOCK_SIZE);
  get_mem3Dint (&(s->ref_pic_id), 2, size_x / BLOCK_SIZE, size_y / BLOCK_SIZE);
  get_mem4Dint (&(s->mv), 2, size_x / BLOCK_SIZE, size_y / BLOCK_SIZE,2 );

  s->pic_num=0;
  s->long_term_frame_idx=0;
  s->long_term_pic_num=0;
  s->used_for_reference=0;
  s->is_long_term=0;

  s->structure=structure;

  s->size_x = size_x;
  s->size_y = size_y;
  s->size_x_cr = size_x_cr;
  s->size_y_cr = size_y_cr;
  
  s->top_field    = NULL;
  s->bottom_field = NULL;
  s->frame        = NULL;

  s->coded_frame                   = 0;
  s->mb_adaptive_frame_field_flag  = 0;

  return s;
}

/*!
 ************************************************************************
 * \brief
 *    Free frame store memory.
 *
 * \param f
 *    FrameStore to be freed
 *
 ************************************************************************
 */
void free_frame_store(FrameStore* f)
{
  if (f)
  {
    if (f->frame)
    {
      free_storable_picture(f->frame);
      f->frame=NULL;
    }
    if (f->top_field)
    {
      free_storable_picture(f->top_field);
      f->top_field=NULL;
    }
    if (f->bottom_field)
    {
      free_storable_picture(f->bottom_field);
      f->bottom_field=NULL;
    }
    free(f);
  }
}

/*!
 ************************************************************************
 * \brief
 *    Free picture memory.
 *
 * \param p
 *    Picture to be freed
 *
 ************************************************************************
 */
void free_storable_picture(StorablePicture* p)
{
  if (p)
  {
    free_mem3Dint (p->ref_idx, 2);
    free_mem3Dint (p->ref_pic_id, 2);
    free_mem4Dint (p->mv, 2, p->size_x / BLOCK_SIZE);
    
    if (p->imgY)
    {
      free_mem2D (p->imgY);
      p->imgY=NULL;
    }
    if (p->imgY_11)
    {
      free (p->imgY_11);
      p->imgY_11=NULL;
    }
    if (p->imgY_ups)
    {
      free_mem2D (p->imgY_ups);
      p->imgY_ups=NULL;
    }
    if (p->imgUV)
    {
      free_mem3D (p->imgUV, 2);
      p->imgUV=NULL;
    }
    free(p);
  }
}

/*!
 ************************************************************************
 * \brief
 *    mark FrameStore unused for reference
 *
 ************************************************************************
 */
static void unmark_for_reference(FrameStore* fs)
{

  if (fs->is_used & 1)
  {
    fs->top_field->used_for_reference = 0;
  }
  if (fs->is_used & 2)
  {
    fs->bottom_field->used_for_reference = 0;
  }
  if (fs->is_used == 3)
  {
    fs->frame->used_for_reference = 0;
  }

  fs->is_reference = 0;
}


/*!
 ************************************************************************
 * \brief
 *    mark FrameStore unused for reference and reset long term flags
 *
 ************************************************************************
 */
static void unmark_for_long_term_reference(FrameStore* fs)
{

  if (fs->is_used & 1)
  {
    fs->top_field->used_for_reference = 0;
    fs->top_field->is_long_term = 0;
  }
  if (fs->is_used & 2)
  {
    fs->bottom_field->used_for_reference = 0;
    fs->bottom_field->is_long_term = 0;
  }
  if (fs->is_used == 3)
  {
    fs->frame->used_for_reference = 0;
    fs->frame->is_long_term = 0;
  }

  fs->is_reference = 0;
  fs->is_long_term = 0;
}


/*!
 ************************************************************************
 * \brief
 *    compares two stored pictures by picture number for qsort in descending order
 *
 ************************************************************************
 */
static int compare_pic_by_pic_num_desc( const void *arg1, const void *arg2 )
{
  if ( (*(StorablePicture**)arg1)->pic_num < (*(StorablePicture**)arg2)->pic_num)
    return 1;
  if ( (*(StorablePicture**)arg1)->pic_num > (*(StorablePicture**)arg2)->pic_num)
    return -1;
  else
    return 0;
}

/*!
 ************************************************************************
 * \brief
 *    compares two stored pictures by picture number for qsort in descending order
 *
 ************************************************************************
 */
static int compare_pic_by_lt_pic_num_asc( const void *arg1, const void *arg2 )
{
  if ( (*(StorablePicture**)arg1)->long_term_pic_num < (*(StorablePicture**)arg2)->long_term_pic_num)
    return -1;
  if ( (*(StorablePicture**)arg1)->long_term_pic_num > (*(StorablePicture**)arg2)->long_term_pic_num)
    return 1;
  else
    return 0;
}

/*!
 ************************************************************************
 * \brief
 *    compares two frame stores by pic_num for qsort in descending order
 *
 ************************************************************************
 */
static int compare_fs_by_frame_num_desc( const void *arg1, const void *arg2 )
{
  if ( (*(FrameStore**)arg1)->frame_num < (*(FrameStore**)arg2)->frame_num)
    return 1;
  if ( (*(FrameStore**)arg1)->frame_num > (*(FrameStore**)arg2)->frame_num)
    return -1;
  else
    return 0;
}


/*!
 ************************************************************************
 * \brief
 *    compares two frame stores by lt_pic_num for qsort in descending order
 *
 ************************************************************************
 */
static int compare_fs_by_lt_pic_idx_asc( const void *arg1, const void *arg2 )
{
  if ( (*(FrameStore**)arg1)->long_term_frame_idx < (*(FrameStore**)arg2)->long_term_frame_idx)
    return -1;
  if ( (*(FrameStore**)arg1)->long_term_frame_idx > (*(FrameStore**)arg2)->long_term_frame_idx)
    return 1;
  else
    return 0;
}


/*!
 ************************************************************************
 * \brief
 *    compares two stored pictures by poc for qsort in ascending order
 *
 ************************************************************************
 */
static int compare_pic_by_poc_asc( const void *arg1, const void *arg2 )
{
  if ( (*(StorablePicture**)arg1)->poc < (*(StorablePicture**)arg2)->poc)
    return -1;
  if ( (*(StorablePicture**)arg1)->poc > (*(StorablePicture**)arg2)->poc)
    return 1;
  else
    return 0;
}


/*!
 ************************************************************************
 * \brief
 *    compares two stored pictures by poc for qsort in descending order
 *
 ************************************************************************
 */
static int compare_pic_by_poc_desc( const void *arg1, const void *arg2 )
{
  if ( (*(StorablePicture**)arg1)->poc < (*(StorablePicture**)arg2)->poc)
    return 1;
  if ( (*(StorablePicture**)arg1)->poc > (*(StorablePicture**)arg2)->poc)
    return -1;
  else
    return 0;
}


/*!
 ************************************************************************
 * \brief
 *    compares two frame stores by poc for qsort in ascending order
 *
 ************************************************************************
 */
static int compare_fs_by_poc_asc( const void *arg1, const void *arg2 )
{
  if ( (*(FrameStore**)arg1)->poc < (*(FrameStore**)arg2)->poc)
    return -1;
  if ( (*(FrameStore**)arg1)->poc > (*(FrameStore**)arg2)->poc)
    return 1;
  else
    return 0;
}


/*!
 ************************************************************************
 * \brief
 *    compares two frame stores by poc for qsort in descending order
 *
 ************************************************************************
 */
static int compare_fs_by_poc_desc( const void *arg1, const void *arg2 )
{
  if ( (*(FrameStore**)arg1)->poc < (*(FrameStore**)arg2)->poc)
    return 1;
  if ( (*(FrameStore**)arg1)->poc > (*(FrameStore**)arg2)->poc)
    return -1;
  else
    return 0;
}


/*!
 ************************************************************************
 * \brief
 *    returns true, if picture is short term reference picture
 *
 ************************************************************************
 */
int is_short_ref(StorablePicture *s)
{
  return ((s->used_for_reference) && (!(s->is_long_term)));
}


/*!
 ************************************************************************
 * \brief
 *    returns true, if picture is long term reference picture
 *
 ************************************************************************
 */
int is_long_ref(StorablePicture *s)
{
  return ((s->used_for_reference) && (s->is_long_term));
}


/*!
 ************************************************************************
 * \brief
 *    Generates a alternating field list from a given FrameStore list
 *
 ************************************************************************
 */
static void gen_pic_list_from_frame_list(PictureStructure currStrcture, FrameStore **fs_list, int list_idx, StorablePicture **list, int *list_size, int long_term)
{
  int top_idx = 0;
  int bot_idx = 0;

  int (*is_ref)(StorablePicture *s);

  if (long_term)
    is_ref=is_long_ref;
  else
    is_ref=is_short_ref;

  if (currStrcture == TOP_FIELD)
  {
    while ((top_idx<list_idx)||(bot_idx<list_idx))
    {
      for ( ; top_idx<list_idx; top_idx++)
      {
        if(fs_list[top_idx]->is_used & 1)
        {
          if(is_ref(fs_list[top_idx]->top_field))
          {
            // short term ref pic
            list[*list_size] = fs_list[top_idx]->top_field;
            (*list_size)++;
            top_idx++;
            break;
          }
        }
      }
      for ( ; bot_idx<list_idx; bot_idx++)
      {
        if(fs_list[bot_idx]->is_used & 2)
        {
          if(is_ref(fs_list[bot_idx]->bottom_field))
          {
            // short term ref pic
            list[*list_size] = fs_list[bot_idx]->bottom_field;
            (*list_size)++;
            bot_idx++;
            break;
          }
        }
      }
    }
  }
  if (currStrcture == BOTTOM_FIELD)
  {
    while ((top_idx<list_idx)||(bot_idx<list_idx))
    {
      for ( ; bot_idx<list_idx; bot_idx++)
      {
        if(fs_list[bot_idx]->is_used & 2)
        {
          if(is_ref(fs_list[bot_idx]->bottom_field))
          {
            // short term ref pic
            list[*list_size] = fs_list[bot_idx]->bottom_field;
            (*list_size)++;
            bot_idx++;
            break;
          }
        }
      }
      for ( ; top_idx<list_idx; top_idx++)
      {
        if(fs_list[top_idx]->is_used & 1)
        {
          if(is_ref(fs_list[top_idx]->top_field))
          {
            // short term ref pic
            list[*list_size] = fs_list[top_idx]->top_field;
            (*list_size)++;
            top_idx++;
            break;
          }
        }
      }
    }
  }
}


/*!
 ************************************************************************
 * \brief
 *    Initilaize listX[0] and list 1 depending on current picture type
 *
 ************************************************************************
 */
void init_lists(int currSliceType, PictureStructure currPicStructure)
{
  int add_top, add_bottom;
  unsigned i;
  int j;
  int MaxFrameNum = 1 << (LOG2_MAX_FRAME_NUM_MINUS4 + 4);
  int diff;

  int list0idx = 0;
  int list1idx = 0;
  int list0idx_1 = 0;
  int listltidx = 0;

  FrameStore **fs_list0;
  FrameStore **fs_list1;
  FrameStore **fs_listlt;

  StorablePicture *tmp_s;

  if ((currSliceType == I_SLICE)||(currSliceType == SI_SLICE))
  {
    listXsize[0] = 0;
    listXsize[1] = 0;
    return;
  }

  if ((currSliceType == P_SLICE)||(currSliceType == SP_SLICE))
  {
    // Calculate FrameNumWrap and PicNum
    if (currPicStructure == FRAME)  
    {
      for (i=0; i<dpb.ref_frames_in_buffer; i++)
      {
        if (dpb.fs_ref[i]->is_used==3)
        {
          if ((dpb.fs_ref[i]->frame->used_for_reference)&&(!dpb.fs_ref[i]->frame->is_long_term))
          {
            if( dpb.fs_ref[i]->frame_num > img->frame_num )
            {
              dpb.fs_ref[i]->frame_num_wrap = dpb.fs_ref[i]->frame_num - MaxFrameNum;
            }
            else
            {
              dpb.fs_ref[i]->frame_num_wrap = dpb.fs_ref[i]->frame_num;
            }
            dpb.fs_ref[i]->frame->pic_num = dpb.fs_ref[i]->frame_num_wrap;
            listX[0][list0idx++] = dpb.fs_ref[i]->frame;
          }
        }
      }
      // order list 0 by PicNum
      qsort((void *)listX[0], list0idx, sizeof(StorablePicture*), compare_pic_by_pic_num_desc);
      listXsize[0] = list0idx;
//      printf("listX[0] (PicNum): "); for (i=0; i<list0idx; i++){printf ("%d  ", listX[0][i]->pic_num);} printf("\n");

      // long term handling
      for (i=0; i<dpb.ltref_frames_in_buffer; i++)
      {
        if (dpb.fs_ltref[i]->is_used==3)
        {
          dpb.fs_ltref[i]->frame->long_term_pic_num = dpb.fs_ltref[i]->frame->long_term_frame_idx;
          // if we have two fields, both must be long-term
          listX[0][list0idx++]=dpb.fs_ltref[i]->frame;
        }
      }
      qsort((void *)&listX[0][listXsize[0]], list0idx-listXsize[0], sizeof(StorablePicture*), compare_pic_by_lt_pic_num_asc);
      listXsize[0] = list0idx;
    }
    else
    {
      fs_list0 = calloc(dpb.size, sizeof (FrameStore*));
      if (NULL==fs_list0) 
         no_mem_exit("init_lists: fs_list0");
      fs_listlt = calloc(dpb.size, sizeof (FrameStore*));
      if (NULL==fs_listlt) 
         no_mem_exit("init_lists: fs_listlt");

      if (currPicStructure == TOP_FIELD)
      {
        add_top    = 1;
        add_bottom = 0;
      }
      else
      {
        add_top    = 0;
        add_bottom = 1;
      }
      
      for (i=0; i<dpb.ref_frames_in_buffer; i++)
      {
        if (dpb.fs_ref[i]->is_reference)
        {
          if( dpb.fs_ref[i]->frame_num > img->frame_num )
          {
            dpb.fs_ref[i]->frame_num_wrap = dpb.fs_ref[i]->frame_num - MaxFrameNum;
          }
          else
          {
            dpb.fs_ref[i]->frame_num_wrap = dpb.fs_ref[i]->frame_num;
          }
          if (dpb.fs_ref[i]->is_reference & 1)
          {
            dpb.fs_ref[i]->top_field->pic_num = (2 * dpb.fs_ref[i]->frame_num_wrap) + add_top;
          }
          if (dpb.fs_ref[i]->is_reference & 2)
          {
            dpb.fs_ref[i]->bottom_field->pic_num = (2 * dpb.fs_ref[i]->frame_num_wrap) + add_bottom;
          }
          fs_list0[list0idx++] = dpb.fs_ref[i];
        }
      }

      qsort((void *)fs_list0, list0idx, sizeof(FrameStore*), compare_fs_by_frame_num_desc);

//      printf("fs_list0 (FrameNum): "); for (i=0; i<list0idx; i++){printf ("%d  ", fs_list0[i]->frame_num);} printf("\n");

      listXsize[0] = 0;
      gen_pic_list_from_frame_list(currPicStructure, fs_list0, list0idx, listX[0], &listXsize[0], 0);

//      printf("listX[0] (PicNum): "); for (i=0; i<listXsize[0]; i++){printf ("%d  ", listX[0][i]->pic_num);} printf("\n");

      // long term handling
      for (i=0; i<dpb.ltref_frames_in_buffer; i++)
      {
        fs_listlt[listltidx++]=dpb.fs_ltref[i];
        if (dpb.fs_ltref[i]->is_long_term & 1)
        {
          dpb.fs_ltref[i]->top_field->long_term_pic_num = 2 * dpb.fs_ltref[i]->top_field->long_term_frame_idx + add_top;
        }
        if (dpb.fs_ltref[i]->is_long_term & 2)
        {
          dpb.fs_ltref[i]->bottom_field->long_term_pic_num = 2 * dpb.fs_ltref[i]->bottom_field->long_term_frame_idx + add_bottom;
        }
      }

      qsort((void *)fs_listlt, listltidx, sizeof(FrameStore*), compare_fs_by_lt_pic_idx_asc);

      gen_pic_list_from_frame_list(currPicStructure, fs_listlt, listltidx, listX[0], &listXsize[0], 1);

      free(fs_list0);
      free(fs_listlt);
    }
    listXsize[1] = 0;
  }
  else
  {
    // B-Slice
    if (currPicStructure == FRAME)  
    {
      for (i=0; i<dpb.ref_frames_in_buffer; i++)
      {
        if (dpb.fs_ref[i]->is_used==3)
        {
          if ((dpb.fs_ref[i]->frame->used_for_reference)&&(!dpb.fs_ref[i]->frame->is_long_term))
          {
            if (img->framepoc > dpb.fs_ref[i]->frame->poc)
            {
              listX[0][list0idx++] = dpb.fs_ref[i]->frame;
            }
          }
        }
      }
      qsort((void *)listX[0], list0idx, sizeof(StorablePicture*), compare_pic_by_poc_desc);
      list0idx_1 = list0idx;
      for (i=0; i<dpb.ref_frames_in_buffer; i++)
      {
        if (dpb.fs_ref[i]->is_used==3)
        {
          if ((dpb.fs_ref[i]->frame->used_for_reference)&&(!dpb.fs_ref[i]->frame->is_long_term))
          {
            if (img->framepoc < dpb.fs_ref[i]->frame->poc)
            {
              listX[0][list0idx++] = dpb.fs_ref[i]->frame;
            }
          }
        }
      }
      qsort((void *)&listX[0][list0idx_1], list0idx-list0idx_1, sizeof(StorablePicture*), compare_pic_by_poc_asc);

      for (j=0; j<list0idx_1; j++)
      {
        listX[1][list0idx-list0idx_1+j]=listX[0][j];
      }
      for (j=list0idx_1; j<list0idx; j++)
      {
        listX[1][list0idx_1-j]=listX[0][j];
      }

      listXsize[0] = listXsize[1] = list0idx;

//      printf("listX[0] currPoc=%d (Poc): ", img->framepoc); for (i=0; i<listXsize[0]; i++){printf ("%d  ", listX[0][i]->poc);} printf("\n");
//      printf("listX[1] currPoc=%d (Poc): ", img->framepoc); for (i=0; i<listXsize[1]; i++){printf ("%d  ", listX[1][i]->poc);} printf("\n");

      // long term handling
      for (i=0; i<dpb.ltref_frames_in_buffer; i++)
      {
        if (dpb.fs_ltref[i]->is_used==3)
        {
          // if we have two fields, both must be long-term
          listX[0][list0idx]  =dpb.fs_ltref[i]->frame;
          listX[1][list0idx++]=dpb.fs_ltref[i]->frame;
        }
      }
      qsort((void *)&listX[0][listXsize[0]], list0idx-listXsize[0], sizeof(StorablePicture*), compare_pic_by_lt_pic_num_asc);
      qsort((void *)&listX[1][listXsize[0]], list0idx-listXsize[0], sizeof(StorablePicture*), compare_pic_by_lt_pic_num_asc);
      listXsize[0] = listXsize[1] = list0idx;
    }
    else
    {
      fs_list0 = calloc(dpb.size, sizeof (FrameStore*));
      if (NULL==fs_list0) 
         no_mem_exit("init_lists: fs_list0");
      fs_list1 = calloc(dpb.size, sizeof (FrameStore*));
      if (NULL==fs_list1) 
         no_mem_exit("init_lists: fs_list1");
      fs_listlt = calloc(dpb.size, sizeof (FrameStore*));
      if (NULL==fs_listlt) 
         no_mem_exit("init_lists: fs_listlt");

      listXsize[0] = 0;
      listXsize[1] = 1;

      for (i=0; i<dpb.ref_frames_in_buffer; i++)
      {
        if (dpb.fs_ref[i]->is_used)
        {
          if (img->ThisPOC > dpb.fs_ref[i]->poc)
          {
            fs_list0[list0idx++] = dpb.fs_ref[i];
          }
        }
      }
      qsort((void *)fs_list0, list0idx, sizeof(FrameStore*), compare_fs_by_poc_desc);
      list0idx_1 = list0idx;
      for (i=0; i<dpb.ref_frames_in_buffer; i++)
      {
        if (dpb.fs_ref[i]->is_used)
        {
          if (img->ThisPOC < dpb.fs_ref[i]->poc)
          {
            fs_list0[list0idx++] = dpb.fs_ref[i];
          }
        }
      }
      qsort((void *)&fs_list0[list0idx_1], list0idx-list0idx_1, sizeof(FrameStore*), compare_fs_by_poc_asc);

      for (j=0; j<list0idx_1; j++)
      {
        fs_list1[list0idx-list0idx_1+j]=fs_list0[j];
      }
      for (j=list0idx_1; j<list0idx; j++)
      {
        fs_list1[list0idx_1-j]=fs_list0[j];
      }
      
//      printf("fs_list0 currPoc=%d (Poc): ", img->ThisPOC); for (i=0; i<list0idx; i++){printf ("%d  ", fs_list0[i]->poc);} printf("\n");
//      printf("fs_list1 currPoc=%d (Poc): ", img->ThisPOC); for (i=0; i<list0idx; i++){printf ("%d  ", fs_list1[i]->poc);} printf("\n");

      listXsize[0] = 0;
      listXsize[1] = 0;
      gen_pic_list_from_frame_list(currPicStructure, fs_list0, list0idx, listX[0], &listXsize[0], 0);
      gen_pic_list_from_frame_list(currPicStructure, fs_list1, list0idx, listX[1], &listXsize[1], 0);

//      printf("listX[0] currPoc=%d (Poc): ", img->framepoc); for (i=0; i<listXsize[0]; i++){printf ("%d  ", listX[0][i]->poc);} printf("\n");
//      printf("listX[1] currPoc=%d (Poc): ", img->framepoc); for (i=0; i<listXsize[1]; i++){printf ("%d  ", listX[1][i]->poc);} printf("\n");

      // long term handling
      for (i=0; i<dpb.ltref_frames_in_buffer; i++)
      {
        fs_listlt[listltidx++]=dpb.fs_ltref[i];
      }

      qsort((void *)fs_listlt, listltidx, sizeof(FrameStore*), compare_fs_by_lt_pic_idx_asc);

      gen_pic_list_from_frame_list(currPicStructure, fs_listlt, listltidx, listX[0], &listXsize[0], 1);
      gen_pic_list_from_frame_list(currPicStructure, fs_listlt, listltidx, listX[1], &listXsize[1], 1);

      free(fs_list0);
      free(fs_list1);
      free(fs_listlt);
    }
  }

  if ((listXsize[0] == listXsize[1]) && (listXsize[0] > 1))
  {
    // check if lists are identical, if yes swap first two elements of listX[1]
    diff=0;
    for (j = 0; j< listXsize[0]; j++)
    {
      if (listX[0][j]!=listX[1][j])
        diff=1;
    }
    if (!diff)
    {
      tmp_s = listX[1][0];
      listX[1][0]=listX[1][1];
      listX[1][1]=tmp_s;
    }
  }

  // set max size
  listXsize[0] = min (listXsize[0], img->num_ref_idx_l0_active);
  listXsize[1] = min (listXsize[1], img->num_ref_idx_l1_active);

  // set the unused list entries to NULL
  for (i=listXsize[0]; i< (2*dpb.size) ; i++)
  {
    listX[0][i] = NULL;
  }
  for (i=listXsize[1]; i< (2*dpb.size) ; i++)
  {
    listX[1][i] = NULL;
  }
}

/*!
 ************************************************************************
 * \brief
 *    Initilaize listX[2..5] from lists 0 and 1
 *    listX[2]: list0 for current_field==top
 *    listX[3]: list1 for current_field==top
 *    listX[4]: list0 for current_field==bottom
 *    listX[5]: list1 for current_field==bottom
 *
 ************************************************************************
 */
void init_mbaff_lists()
{
  unsigned j;
  int i;

  for (i=2;i<6;i++)
  {
    for (j=0; j<(2*dpb.size)+1; j++)
    {
      listX[i][j] = NULL;
    }
    listXsize[i]=0;
  }

  for (i=0; i<listXsize[0]; i++)
  {
    listX[2][2*i]  =listX[0][i]->top_field;
    listX[2][2*i+1]=listX[0][i]->bottom_field;
    listX[4][2*i]  =listX[0][i]->bottom_field;
    listX[4][2*i+1]=listX[0][i]->top_field;
  }
  listXsize[2]=listXsize[4]=listXsize[0] * 2;

  for (i=0; i<listXsize[1]; i++)
  {
    listX[3][2*i]  =listX[1][i]->top_field;
    listX[3][2*i+1]=listX[1][i]->bottom_field;
    listX[5][2*i]  =listX[1][i]->bottom_field;
    listX[5][2*i+1]=listX[1][i]->top_field;
  }
  listXsize[3]=listXsize[5]=listXsize[1] * 2;
}
 
 /*!
 ************************************************************************
 * \brief
 *    Returns short term pic with given picNum
 *
 ************************************************************************
 */
static StorablePicture*  get_short_term_pic(int picNum)
{
  unsigned i;

  for (i=0; i<dpb.ref_frames_in_buffer; i++)
  {
    if (img->type==FRAME)
    {
      if (dpb.fs_ref[i]->is_reference == 3)
        if ((!dpb.fs_ref[i]->frame->is_long_term)&&(dpb.fs_ref[i]->frame->pic_num == picNum))
          return dpb.fs_ref[i]->frame;
    }
    else
    {
      if (dpb.fs_ref[i]->is_reference & 1)
        if ((!dpb.fs_ref[i]->top_field->is_long_term)&&(dpb.fs_ref[i]->top_field->pic_num == picNum))
          return dpb.fs_ref[i]->top_field;
      if (dpb.fs_ref[i]->is_reference & 2)
        if ((!dpb.fs_ref[i]->bottom_field->is_long_term)&&(dpb.fs_ref[i]->bottom_field->pic_num == picNum))
          return dpb.fs_ref[i]->bottom_field;
    }
  }
  return NULL;
}

/*!
 ************************************************************************
 * \brief
 *    Returns short term pic with given LongtermPicNum
 *
 ************************************************************************
 */
static StorablePicture*  get_long_term_pic(int LongtermPicNum)
{
  unsigned i;

  for (i=0; i<dpb.ref_frames_in_buffer; i++)
  {
    if (img->type==FRAME)
    {
      if (dpb.fs_ref[i]->is_reference == 3)
        if ((dpb.fs_ref[i]->frame->is_long_term)&&(dpb.fs_ref[i]->frame->long_term_pic_num == LongtermPicNum))
          return dpb.fs_ref[i]->frame;
    }
    else
    {
      if (dpb.fs_ref[i]->is_reference & 1)
        if ((dpb.fs_ref[i]->top_field->is_long_term)&&(dpb.fs_ref[i]->top_field->long_term_pic_num == LongtermPicNum))
          return dpb.fs_ref[i]->top_field;
      if (dpb.fs_ref[i]->is_reference & 2)
        if ((dpb.fs_ref[i]->bottom_field->is_long_term)&&(dpb.fs_ref[i]->bottom_field->long_term_pic_num == LongtermPicNum))
          return dpb.fs_ref[i]->bottom_field;
    }
  }
  return NULL;
}

/*!
 ************************************************************************
 * \brief
 *    Reordering process for short-term reference pictures
 *
 ************************************************************************
 */
static void reorder_short_term(StorablePicture **RefPicListX, int num_ref_idx_lX_active_minus1, int picNumLX, int *refIdxLX)
{
  int cIdx, nIdx;

  StorablePicture *picLX;

  picLX = get_short_term_pic(picNumLX);

  for( cIdx = num_ref_idx_lX_active_minus1+1; cIdx > *refIdxLX; cIdx-- )
    RefPicListX[ cIdx ] = RefPicListX[ cIdx - 1];
  
  RefPicListX[ (*refIdxLX)++ ] = picLX;

  nIdx = *refIdxLX;

  for( cIdx = *refIdxLX; cIdx <= num_ref_idx_lX_active_minus1+1; cIdx++ )
    if( (RefPicListX[ cIdx ]->is_long_term ) ||  (RefPicListX[ cIdx ]->pic_num != picNumLX ))
	    RefPicListX[ nIdx++ ] = RefPicListX[ cIdx ];

}


/*!
 ************************************************************************
 * \brief
 *    Reordering process for short-term reference pictures
 *
 ************************************************************************
 */
static void reorder_long_term(StorablePicture **RefPicListX, int num_ref_idx_lX_active_minus1, int LongTermPicNum, int *refIdxLX)
{
  int cIdx, nIdx;

  StorablePicture *picLX;

  picLX = get_long_term_pic(LongTermPicNum);

  for( cIdx = num_ref_idx_lX_active_minus1+1; cIdx > *refIdxLX; cIdx-- )
    RefPicListX[ cIdx ] = RefPicListX[ cIdx - 1];
  
  RefPicListX[ (*refIdxLX)++ ] = picLX;

  nIdx = *refIdxLX;

  for( cIdx = *refIdxLX; cIdx <= num_ref_idx_lX_active_minus1+1; cIdx++ )
    if( (!RefPicListX[ cIdx ]->is_long_term ) ||  (RefPicListX[ cIdx ]->long_term_pic_num != LongTermPicNum ))
	    RefPicListX[ nIdx++ ] = RefPicListX[ cIdx ];
}


/*!
 ************************************************************************
 * \brief
 *    Reordering process for reference picture lists
 *
 ************************************************************************
 */
void reorder_ref_pic_list(StorablePicture **list, int *list_size, int num_ref_idx_lX_active_minus1, int *remapping_of_pic_nums_idc, int *abs_diff_pic_num_minus1, int *long_term_pic_idx)
{
  int i;

  int maxPicNum, currPicNum, picNumLXNoWrap, picNumLXPred, picNumLX;
  int refIdxLX = 0;
  int MaxFrameNum = 1 << (LOG2_MAX_FRAME_NUM_MINUS4 + 4);

  if (img->structure==FRAME)
  {
    maxPicNum  = MaxFrameNum;
    currPicNum = img->frame_num;
  }
  else
  {
    maxPicNum  = 2 * MaxFrameNum;
    currPicNum = 2 * img->frame_num;
  }

  picNumLXPred = currPicNum;

  for (i=0; remapping_of_pic_nums_idc[i]!=3; i++)
  {
    if (remapping_of_pic_nums_idc[i]>3)
      error ("Invalid remapping_of_pic_nums_idc command", 500);

    if (remapping_of_pic_nums_idc[i] < 2)
    {
      if (remapping_of_pic_nums_idc[i] == 0)
      {
        if( picNumLXPred - ( abs_diff_pic_num_minus1[i] + 1 ) < 0 )
          picNumLXNoWrap = picNumLXPred - ( abs_diff_pic_num_minus1[i] + 1 ) + maxPicNum;
        else
          picNumLXNoWrap = picNumLXPred - ( abs_diff_pic_num_minus1[i] + 1 );
      }
      else // (remapping_of_pic_nums_idc[i] == 1)
      {
        if( picNumLXPred + ( abs_diff_pic_num_minus1[i] + 1 )  >=  maxPicNum )
          picNumLXNoWrap = picNumLXPred + ( abs_diff_pic_num_minus1[i] + 1 ) - maxPicNum;
        else
          picNumLXNoWrap = picNumLXPred + ( abs_diff_pic_num_minus1[i] + 1 );
      }
      picNumLXPred = picNumLXNoWrap;

      if( picNumLXNoWrap > currPicNum )
        picNumLX = picNumLXNoWrap - maxPicNum;
      else
        picNumLX = picNumLXNoWrap;

      reorder_short_term(list, num_ref_idx_lX_active_minus1, picNumLX, &refIdxLX);
    }
    else //(remapping_of_pic_nums_idc[i] == 2)
    {
      reorder_long_term(list, num_ref_idx_lX_active_minus1, long_term_pic_idx[i], &refIdxLX);
    }
    
  }
  // that's a definition
  *list_size = num_ref_idx_lX_active_minus1 + 1;
}



/*!
 ************************************************************************
 * \brief
 *    Update the list of frame stores that contain reference frames/fields
 *
 ************************************************************************
 */
void update_ref_list()
{
  unsigned i, j;
  for (i=0, j=0; i<dpb.used_size; i++)
  {
    if (is_short_term_reference(dpb.fs[i]))
    {
      dpb.fs_ref[j++]=dpb.fs[i];
    }
  }

  dpb.ref_frames_in_buffer = j;

  while (j<dpb.size)
  {
    dpb.fs_ref[j++]=NULL;
  }
}


/*!
 ************************************************************************
 * \brief
 *    Update the list of frame stores that contain long-term reference 
 *    frames/fields
 *
 ************************************************************************
 */
void update_ltref_list()
{
  unsigned i, j;
  for (i=0, j=0; i<dpb.used_size; i++)
  {
    if (is_long_term_reference(dpb.fs[i]))
    {
      dpb.fs_ltref[j++]=dpb.fs[i];
    }
  }

  dpb.ltref_frames_in_buffer=j;

  while (j<dpb.size)
  {
    dpb.fs_ltref[j++]=NULL;
  }
}

/*!
 ************************************************************************
 * \brief
 *    Perform Memory management for idr pictures
 *
 ************************************************************************
 */
static void idr_memory_management(StorablePicture* p)
{
  unsigned i;

  assert (img->currentPicture->idr_flag);

  if (img->no_output_of_prior_pics_flag)
  {
    // free all stored pictures
    for (i=0; i<dpb.used_size; i++)
    {
      free_frame_store(dpb.fs[i]);
      dpb.fs[i]=NULL;
    }
    for (i=0; i<dpb.ref_frames_in_buffer; i++)
    {
      dpb.fs_ref[i]=NULL;
    }
    for (i=0; i<dpb.ltref_frames_in_buffer; i++)
    {
      dpb.fs_ltref[i]=NULL;
    }
    dpb.used_size=0;
  }
  else
  {
    flush_dpb();
  }

  update_ref_list();
  update_ltref_list();
  dpb.last_output_poc = INT_MIN;
  
  if (img->long_term_reference_flag)
  {
    dpb.max_long_term_pic_idx = 0;
    p->is_long_term           = 1;
    p->long_term_frame_idx    = 0;
  }
  else
  {
    dpb.max_long_term_pic_idx = -1;
    p->is_long_term           = 0;
  }
}

/*!
 ************************************************************************
 * \brief
 *    Perform Sliding window decoded reference picture marking process
 *
 ************************************************************************
 */
static void sliding_window_memory_management(StorablePicture* p)
{
  unsigned i;

  assert (!img->currentPicture->idr_flag);
  // if this is a reference pic with sliding sliding window, unmark first ref frame
  //if ((int)dpb.ref_frames_in_buffer==active_sps->num_ref_frames)
  if ((int)dpb.ref_frames_in_buffer==input->num_reference_frames)
  {
    for (i=0; i<dpb.used_size;i++)
    {
      if (dpb.fs[i]->is_reference)
      {
        unmark_for_reference(dpb.fs[i]);
        update_ref_list();
        break;
      }
    }
  }

  p->is_long_term = 0;
}

/*!
 ************************************************************************
 * \brief
 *    Calculate picNumX
 ************************************************************************
 */
static get_pic_num_x (StorablePicture *p, int difference_of_pic_nums_minus1)
{
  int currPicNum;

  if (p->structure == FRAME)
    currPicNum = img->frame_num;
  else 
    currPicNum = 2 * img->frame_num;
  
  return currPicNum - (difference_of_pic_nums_minus1 + 1);
}


/*!
 ************************************************************************
 * \brief
 *    Adaptive Memory Management: Mark short term picture unused
 ************************************************************************
 */
static void mm_unmark_short_term_for_reference(StorablePicture *p, int difference_of_pic_nums_minus1)
{
  int picNumX;

  unsigned i;

  picNumX = get_pic_num_x(p, difference_of_pic_nums_minus1);

  for (i=0; i<dpb.ref_frames_in_buffer; i++)
  {
    if (p->structure == FRAME)
    {
      if ((dpb.fs_ref[i]->is_reference==3) && (dpb.fs_ref[i]->is_long_term==0))
      {
        if (dpb.fs_ref[i]->frame->pic_num == picNumX)
        {
          unmark_for_reference(dpb.fs_ref[i]);
          return;
        }
      }
    }
    else
    {
      if ((dpb.fs_ref[i]->is_reference & 1) && (!(dpb.fs_ref[i]->is_long_term & 1)))
      {
        if (dpb.fs_ref[i]->top_field->pic_num == picNumX)
        {
          dpb.fs_ref[i]->top_field->used_for_reference = 0;
          dpb.fs_ref[i]->is_reference &= 2;
          if (dpb.fs_ref[i]->is_used == 3)
          {
            dpb.fs_ref[i]->frame->used_for_reference = 0;
          }
          return;
        }
      }
      if ((dpb.fs_ref[i]->is_reference & 2) && (!(dpb.fs_ref[i]->is_long_term & 2)))
      {
        if (dpb.fs_ref[i]->bottom_field->pic_num == picNumX)
        {
          dpb.fs_ref[i]->bottom_field->used_for_reference = 0;
          dpb.fs_ref[i]->is_reference &= 1;
          if (dpb.fs_ref[i]->is_used == 3)
          {
            dpb.fs_ref[i]->frame->used_for_reference = 0;
          }
          return;
        }
      }
    }
  }
}


/*!
 ************************************************************************
 * \brief
 *    Adaptive Memory Management: Mark long term picture unused
 ************************************************************************
 */
static void mm_unmark_long_term_for_reference(StorablePicture *p, int long_term_pic_num)
{
  unsigned i;
  for (i=0; i<dpb.ltref_frames_in_buffer; i++)
  {
    if (p->structure == FRAME)
    {
      if ((dpb.fs_ltref[i]->is_reference==3) && (dpb.fs_ltref[i]->is_long_term==3))
      {
        if (dpb.fs_ltref[i]->frame->long_term_pic_num == long_term_pic_num)
        {
          unmark_for_long_term_reference(dpb.fs_ltref[i]);
        }
      }
    }
    else
    {
      if ((dpb.fs_ltref[i]->is_reference & 1) && ((dpb.fs_ltref[i]->is_long_term & 1)))
      {
        if (dpb.fs_ltref[i]->top_field->long_term_pic_num == long_term_pic_num)
        {
          dpb.fs_ltref[i]->top_field->used_for_reference = 0;
          dpb.fs_ltref[i]->top_field->is_long_term = 0;
          dpb.fs_ltref[i]->is_reference &= 2;
          dpb.fs_ltref[i]->is_long_term &= 2;
          if (dpb.fs_ltref[i]->is_used == 3)
          {
            dpb.fs_ltref[i]->frame->used_for_reference = 0;
            dpb.fs_ltref[i]->frame->is_long_term = 0;
          }
          return;
        }
      }
      if ((dpb.fs_ltref[i]->is_reference & 2) && ((dpb.fs_ltref[i]->is_long_term & 2)))
      {
        if (dpb.fs_ltref[i]->bottom_field->long_term_pic_num == long_term_pic_num)
        {
          dpb.fs_ltref[i]->bottom_field->used_for_reference = 0;
          dpb.fs_ltref[i]->bottom_field->is_long_term = 0;
          dpb.fs_ltref[i]->is_reference &= 1;
          dpb.fs_ltref[i]->is_long_term &= 1;
          if (dpb.fs_ltref[i]->is_used == 3)
          {
            dpb.fs_ltref[i]->frame->used_for_reference = 0;
            dpb.fs_ltref[i]->frame->is_long_term = 0;
          }
          return;
        }
      }
    }
  }
}


/*!
 ************************************************************************
 * \brief
 *    Mark a long-term reference frame or complementary field pair unused for referemce
 ************************************************************************
 */
static void unmark_long_term_frame_for_reference_by_frame_idx(int long_term_frame_idx)
{
  unsigned i;
  for(i=0; i<dpb.ref_frames_in_buffer; i++)
  {
    if (dpb.fs_ref[i]->long_term_frame_idx == long_term_frame_idx)
      unmark_for_long_term_reference(dpb.fs_ltref[i]);
  }
}

/*!
 ************************************************************************
 * \brief
 *    Mark a long-term reference field unused for referemce only if it's not
 *    the complementary field of the picture indicated by picNumX
 ************************************************************************
 */
static void unmark_long_term_field_for_reference_by_frame_idx(StorablePicture *p, int long_term_frame_idx, int picNumX)
{
  unsigned i;
  for(i=0; i<dpb.ref_frames_in_buffer; i++)
  {
    if (dpb.fs_ref[i]->long_term_frame_idx == long_term_frame_idx)
    {
      if (p->structure == TOP_FIELD)
      {
        if (!(dpb.fs_ref[i]->is_reference == 3))
        {
          unmark_for_long_term_reference(dpb.fs_ltref[i]);
        }
        else
        {
          if (!(dpb.fs_ref[i]->is_long_term == 2))
          {
            unmark_for_long_term_reference(dpb.fs_ltref[i]);
          }
          else
          {
            if (!(dpb.fs_ref[i]->top_field->pic_num==picNumX))
              unmark_for_long_term_reference(dpb.fs_ltref[i]);
          }
        }
      }
      if (p->structure == BOTTOM_FIELD)
      {
        if (!(dpb.fs_ref[i]->is_reference == 3))
        {
          unmark_for_long_term_reference(dpb.fs_ltref[i]);
        }
        else
        {
          if (!(dpb.fs_ref[i]->is_long_term == 1))
          {
            unmark_for_long_term_reference(dpb.fs_ltref[i]);
          }
          else
          {
            if (!(dpb.fs_ref[i]->bottom_field->pic_num==picNumX))
            {
              unmark_for_long_term_reference(dpb.fs_ltref[i]);
            }
          }
        }
      }
    }
  }
}


/*!
 ************************************************************************
 * \brief
 *    mark a picture as long-term reference
 ************************************************************************
 */
static void mark_pic_long_term(StorablePicture* p, int long_term_frame_idx, int picNumX)
{
  unsigned i;
  int add_top, add_bottom;

  if (p->structure == FRAME)
  {
    for (i=0; i<dpb.ref_frames_in_buffer; i++)
    {
      if (dpb.fs_ref[i]->is_reference == 3)
      {
        if ((!dpb.fs_ref[i]->frame->is_long_term)&&(dpb.fs_ref[i]->frame->pic_num == picNumX))
        {
          dpb.fs_ref[i]->long_term_frame_idx = dpb.fs_ref[i]->frame->long_term_frame_idx
                                             = dpb.fs_ref[i]->top_field->long_term_frame_idx
                                             = dpb.fs_ref[i]->bottom_field->long_term_frame_idx
                                             = long_term_frame_idx;
          dpb.fs_ref[i]->frame->long_term_pic_num = dpb.fs_ref[i]->top_field->long_term_pic_num
                                                  = dpb.fs_ref[i]->bottom_field->long_term_pic_num
                                                  = long_term_frame_idx;
          dpb.fs_ref[i]->frame->is_long_term = dpb.fs_ref[i]->top_field->is_long_term
                                             = dpb.fs_ref[i]->bottom_field->is_long_term
                                             = 1;
          dpb.fs_ref[i]->is_long_term = 3;
          return;
        }
      }
    }
    printf ("Warning: reference frame for long term marking not found\n");
  }
  else
  {
    if (p->structure == TOP_FIELD)
    {
      add_top    = 1;
      add_bottom = 0;
    }
    else
    {
      add_top    = 0;
      add_bottom = 1;
    }
    for (i=0; i<dpb.ref_frames_in_buffer; i++)
    {
      if (dpb.fs_ref[i]->is_reference & 1)
      {
        if ((!dpb.fs_ref[i]->top_field->is_long_term)&&(dpb.fs_ref[i]->top_field->pic_num == picNumX))
        {
          if ((dpb.fs_ref[i]->is_long_term) && (dpb.fs_ref[i]->long_term_frame_idx != long_term_frame_idx))
          {
              printf ("Warning: assigning long_term_frame_idx different from other field\n");
          }

          dpb.fs_ref[i]->long_term_frame_idx = dpb.fs_ref[i]->top_field->long_term_frame_idx 
                                             = long_term_frame_idx;
          dpb.fs_ref[i]->top_field->long_term_pic_num = 2 * long_term_frame_idx + add_top;
          dpb.fs_ref[i]->top_field->is_long_term = 1;
          dpb.fs_ref[i]->is_long_term |= 1;
          if (dpb.fs_ref[i]->is_long_term == 3)
          {
            dpb.fs_ref[i]->frame->is_long_term = 1;
            dpb.fs_ref[i]->frame->long_term_frame_idx = dpb.fs_ref[i]->frame->long_term_pic_num = long_term_frame_idx;
          }
          return;
        }
      }
      if (dpb.fs_ref[i]->is_reference & 2)
      {
        if ((!dpb.fs_ref[i]->bottom_field->is_long_term)&&(dpb.fs_ref[i]->bottom_field->pic_num == picNumX))
        {
          if ((dpb.fs_ref[i]->is_long_term) && (dpb.fs_ref[i]->long_term_frame_idx != long_term_frame_idx))
          {
              printf ("Warning: assigning long_term_frame_idx different from other field\n");
          }

          dpb.fs_ref[i]->long_term_frame_idx = dpb.fs_ref[i]->bottom_field->long_term_frame_idx 
                                             = long_term_frame_idx;
          dpb.fs_ref[i]->bottom_field->long_term_pic_num = 2 * long_term_frame_idx + add_top;
          dpb.fs_ref[i]->bottom_field->is_long_term = 1;
          dpb.fs_ref[i]->is_long_term |= 2;
          if (dpb.fs_ref[i]->is_long_term == 3)
          {
            dpb.fs_ref[i]->frame->is_long_term = 1;
            dpb.fs_ref[i]->frame->long_term_frame_idx = dpb.fs_ref[i]->frame->long_term_pic_num = long_term_frame_idx;
          }
          return;
        }
      }
    }
    printf ("Warning: reference field for long term marking not found\n");
  }
}


/*!
 ************************************************************************
 * \brief
 *    Assign a long term frame index to a short term picture
 ************************************************************************
 */
static void mm_assign_long_term_frame_idx(StorablePicture* p, int difference_of_pic_nums_minus1, int long_term_frame_idx)
{
  int picNumX;

  picNumX = get_pic_num_x(p, difference_of_pic_nums_minus1);

  // remove frames/fields with same long_term_frame_idx
  if (p->structure == FRAME)
  {
    unmark_long_term_frame_for_reference_by_frame_idx(long_term_frame_idx);
  }
  else
  {
    unmark_long_term_field_for_reference_by_frame_idx(p, long_term_frame_idx, picNumX);
  }

  mark_pic_long_term(p, long_term_frame_idx, picNumX);
}

/*!
 ************************************************************************
 * \brief
 *    Set new max long_term_frame_idx
 ************************************************************************
 */
void mm_update_max_long_term_frame_idx(int max_long_term_frame_idx_plus1)
{
  unsigned i;

  dpb.max_long_term_pic_idx = max_long_term_frame_idx_plus1 - 1;

  // check for invalid frames
  for (i=0; i<dpb.ltref_frames_in_buffer; i++)
  {
    if (dpb.fs_ltref[i]->long_term_frame_idx > dpb.max_long_term_pic_idx)
    {
      unmark_for_long_term_reference(dpb.fs_ltref[i]);
    }
  }
}


/*!
 ************************************************************************
 * \brief
 *    Mark all long term reference pictures unused for reference
 ************************************************************************
 */
static void mm_unmark_all_long_term_for_reference ()
{
  mm_update_max_long_term_frame_idx(0);
}


/*!
 ************************************************************************
 * \brief
 *    Mark the current picture used for long term reference
 ************************************************************************
 */
static void mm_mark_current_picture_long_term(StorablePicture *p, int long_term_frame_idx)
{
  // remove long term pictures with same long_term_frame_idx
  if (p->structure == FRAME)
  {
    unmark_long_term_frame_for_reference_by_frame_idx(long_term_frame_idx);
  }
  else
  {
    unmark_long_term_field_for_reference_by_frame_idx(p, long_term_frame_idx, 2 * p->pic_num + 1);
  }

  p->is_long_term = 1;
  p->long_term_frame_idx = long_term_frame_idx;
}


/*!
 ************************************************************************
 * \brief
 *    Perform Adaptive memory control decoded reference picture marking process
 ************************************************************************
 */
static void adaptive_memory_management(StorablePicture* p)
{
  DecRefPicMarking_t *tmp_drpm;

  assert (!img->currentPicture->idr_flag);
  assert (!img->adaptive_ref_pic_buffering_flag);

  while (img->dec_ref_pic_marking_buffer)
  {
    tmp_drpm = img->dec_ref_pic_marking_buffer;
    switch (tmp_drpm->memory_management_control_operation)
    {
      case 0:
        if (tmp_drpm->Next != NULL)
        {
          error ("memory_management_control_operation = 0 not last operation in buffer", 500);
        }
        break;
      case 1:
        mm_unmark_short_term_for_reference(p, tmp_drpm->difference_of_pic_nums_minus1);
        update_ref_list();
        break;
      case 2:
        mm_unmark_long_term_for_reference(p, tmp_drpm->long_term_pic_num);
        update_ltref_list();
        break;
      case 3:
        mm_assign_long_term_frame_idx(p, tmp_drpm->difference_of_pic_nums_minus1, tmp_drpm->long_term_frame_idx);
        update_ref_list();
        update_ltref_list();
        break;
      case 4:
        mm_update_max_long_term_frame_idx (tmp_drpm->max_long_term_frame_idx_plus1);
        update_ltref_list();
        break;
      case 5:
        mm_unmark_all_long_term_for_reference();
        break;
      case 6:
        mm_mark_current_picture_long_term(p, tmp_drpm->long_term_frame_idx);
        break;
      default:
        error ("invalid memory_management_control_operation in buffer", 500);
    }
    img->dec_ref_pic_marking_buffer = tmp_drpm->Next;
    free (tmp_drpm);
  }
}


/*!
 ************************************************************************
 * \brief
 *    Store a picture in DPB. This includes cheking for space in DPB and 
 *    flushing frames.
 *    If we received a frame, we need to check for a new store, if we
 *    got a field, check if it's the second field of an already allocated
 *    store.
 *
 * \param p
 *    Picture to be stored
 *
 ************************************************************************
 */
void store_picture_in_dpb(StorablePicture* p)
{
  unsigned i;
  int found;
  int poc, pos;
  // diagnostics
  //printf ("Storing (%s) non-ref pic with frame_num #%d\n", (p->type == FRAME)?"FRAME":(p->type == TOP_FIELD)?"TOP_FIELD":"BOTTOM_FIELD", img->frame_num);
  // if frame, check for new store, 
  assert (p!=NULL);

  p->used_for_reference = (img->nal_reference_idc != 0);
  
  if (img->currentPicture->idr_flag)
    idr_memory_management(p);
  else
  {
    // adaptive memory management
    if (p->used_for_reference && (img->adaptive_ref_pic_buffering_flag))
      adaptive_memory_management(p);
  }

  if ((p->structure==TOP_FIELD)||(p->structure==BOTTOM_FIELD))
  {
    // check for frame store with same pic_number
    found = 0;
    for (i=0;i<dpb.used_size;i++)
    {
      if((dpb.fs[i]->frame_num == img->frame_num)&&(dpb.fs[i]->is_used<3))
      {
        found=1;
        break;
      }
    }
    if (found)
    {
      if (!(((p->structure==TOP_FIELD)&&(dpb.fs[i]->is_used&1))||((p->structure==BOTTOM_FIELD)&&(dpb.fs[i]->is_used&2))))
      {
//        printf ("store second field at pos %d\n",i);
        insert_picture_in_dpb(dpb.fs[i], p);
        update_ref_list();
        update_ltref_list();
        dump_dpb();
        return;
      }
    }
  }
  
  // this is a frame or a field which has no stored complementatry field

  // sliding window, if necessary
  if ((!img->currentPicture->idr_flag)&&(p->used_for_reference && (!img->adaptive_ref_pic_buffering_flag)))
  {
    sliding_window_memory_management(p);
  } 

  // first try to remove unused frames
  if (dpb.used_size==dpb.size)
  {
    remove_unused_frame_from_dpb();
  }
  
  // then output frames until one can be removed
  while (dpb.used_size==dpb.size)
  {
    // non-reference frames may be output directly
    if (!p->used_for_reference)
    {
      get_smallest_poc(&poc, &pos);
      if ((-1==pos) || (p->poc < poc))
      {
        direct_output(p, p_dec);
        return;
      }
    }
    // flush a frame
    output_one_frame_from_dpb();
  }
  
  // check for duplicate frame number in short term reference buffer
  if ((p->used_for_reference)&&(!p->is_long_term))
  {
    for (i=0; i<dpb.ref_frames_in_buffer; i++)
    {
      if (dpb.fs_ref[i]->frame_num == img->frame_num)
      {
        error("duplicate frame_num im short-term reference picture buffer", 500);
      }
    }

  }
  // store at end of buffer
//  printf ("store frame/field at pos %d\n",dpb.used_size);
  insert_picture_in_dpb(dpb.fs[dpb.used_size],p);
  dpb.used_size++;

  update_ref_list();
  update_ltref_list();
  dump_dpb();
}

/*!
 ************************************************************************
 * \brief
 *    Insert the picture into the DPB. A free DPB position is necessary
 *    for frames, .
 *
 * \param p
 *    Picture to be freed
 *
 ************************************************************************
 */
static void insert_picture_in_dpb(FrameStore* fs, StorablePicture* p)
{
//  printf ("insert (%s) pic with frame_num #%d, poc %d\n", (p->structure == FRAME)?"FRAME":(p->structure == TOP_FIELD)?"TOP_FIELD":"BOTTOM_FIELD", img->frame_num, p->poc);
  assert (p!=NULL);
  assert (fs!=NULL);

  // upsample a reference picture
  if (p->used_for_reference)
  {
    UnifiedOneForthPix(p);
  }

  switch (p->structure)
  {
  case FRAME: 
    fs->frame = p;
    fs->is_used = 3;
    if (p->used_for_reference)
    {
      fs->is_reference = 3;
      if (p->is_long_term)
      {
        fs->is_long_term = 3;
      }
    }
    // generate field views
    dpb_split_field(fs);
    break;
  case TOP_FIELD:
    fs->top_field = p;
    fs->is_used |= 1;
    if (p->used_for_reference)
    {
      fs->is_reference |= 1;
      if (p->is_long_term)
      {
        fs->is_long_term |= 1;
        fs->long_term_frame_idx = p->long_term_frame_idx;
      }
    }
    if (fs->is_used == 3)
    {
      // generate frame view
      dpb_combine_field(fs);
    } else
    {
      fs->poc = p->poc;
    }
    break;
  case BOTTOM_FIELD:
    fs->bottom_field = p;
    fs->is_used |= 2;
    if (p->used_for_reference)
    {
      fs->is_reference |= 2;
      if (p->is_long_term)
      {
        fs->is_long_term |= 2;
        fs->long_term_frame_idx = p->long_term_frame_idx;
      }
    }
    if (fs->is_used == 3)
    {
      // generate frame view
      dpb_combine_field(fs);
    } else
    {
      fs->poc = p->poc;
    }
    break;
  }
  fs->frame_num = img->frame_num;
  fs->is_output = 0;

}

/*!
 ************************************************************************
 * \brief
 *    Check if one of the frames/fields in frame store is used for reference
 ************************************************************************
 */
static int is_used_for_reference(FrameStore* fs)
{
  if (fs->is_reference)
  {
    return 1;
  }
  
  if (fs->is_used==3) // frame
  {
    if (fs->frame->used_for_reference)
    {
      return 1;
    }
  }

  if (fs->is_used&1) // top field
  {
    if (fs->top_field->used_for_reference)
    {
      return 1;
    }
  }

  if (fs->is_used&2) // bottom field
  {
    if (fs->bottom_field->used_for_reference)
    {
      return 1;
    }
  }
  return 0;
}


/*!
 ************************************************************************
 * \brief
 *    Check if one of the frames/fields in frame store is used for short-term reference
 ************************************************************************
 */
static int is_short_term_reference(FrameStore* fs)
{

  if (fs->is_used==3) // frame
  {
    if ((fs->frame->used_for_reference)&&(!fs->frame->is_long_term))
    {
      return 1;
    }
  }

  if (fs->is_used&1) // top field
  {
    if ((fs->top_field->used_for_reference)&&(!fs->top_field->is_long_term))
    {
      return 1;
    }
  }

  if (fs->is_used&2) // bottom field
  {
    if ((fs->bottom_field->used_for_reference)&&(!fs->top_field->is_long_term))
    {
      return 1;
    }
  }
  return 0;
}


/*!
 ************************************************************************
 * \brief
 *    Check if one of the frames/fields in frame store is used for short-term reference
 ************************************************************************
 */
static int is_long_term_reference(FrameStore* fs)
{

  if (fs->is_used==3) // frame
  {
    if ((fs->frame->used_for_reference)&&(fs->frame->is_long_term))
    {
      return 1;
    }
  }

  if (fs->is_used&1) // top field
  {
    if ((fs->top_field->used_for_reference)&&(fs->top_field->is_long_term))
    {
      return 1;
    }
  }

  if (fs->is_used&2) // bottom field
  {
    if ((fs->bottom_field->used_for_reference)&&(fs->top_field->is_long_term))
    {
      return 1;
    }
  }
  return 0;
}


/*!
 ************************************************************************
 * \brief
 *    remove one frame from DPB
 ************************************************************************
 */
static void remove_frame_from_dpb(int pos)
{
  FrameStore* fs = dpb.fs[pos];
  FrameStore* tmp;
  unsigned i;
  
//  printf ("remove frame with frame_num #%d\n", fs->frame_num);
  switch (fs->is_used)
  {
  case 3:
    free_storable_picture(fs->frame);
    free_storable_picture(fs->top_field);
    free_storable_picture(fs->bottom_field);
    fs->frame=NULL;
    fs->top_field=NULL;
    fs->bottom_field=NULL;
    break;
  case 2:
    free_storable_picture(fs->bottom_field);
    fs->bottom_field=NULL;
    break;
  case 1:
    free_storable_picture(fs->top_field);
    fs->top_field=NULL;
    break;
  case 0:
    break;
  default:
    error("invalid frame store type",500);
  }
  fs->is_used = 0;

  // move empty framestore to end of buffer
  tmp = dpb.fs[pos];

  for (i=pos; i<dpb.used_size-1;i++)
  {
    dpb.fs[i] = dpb.fs[i+1];
  }
  dpb.fs[dpb.used_size-1] = tmp;
  dpb.used_size--;
}

/*!
 ************************************************************************
 * \brief
 *    find smallest POC in the DPB.
 ************************************************************************
 */
static void get_smallest_poc(int *poc,int * pos)
{
  unsigned i;

  if (dpb.used_size<1)
  {
    error("Cannot determine smallest POC, DPB empty.",150);
  }

  *pos=-1;
  *poc = INT_MAX;
  for (i=0; i<dpb.used_size; i++)
  {
    if ((*poc>dpb.fs[i]->poc)&&(!dpb.fs[i]->is_output))
    {
      *poc = dpb.fs[i]->poc;
      *pos=i;
    }
  }
}

/*!
 ************************************************************************
 * \brief
 *    Remove a picture from DPB which is no longer needed.
 ************************************************************************
 */
static int remove_unused_frame_from_dpb()
{
  unsigned i;

  // check for frames that were already output and no longer used for reference
  for (i=0; i<dpb.used_size; i++)
  {
    if (dpb.fs[i]->is_output && (!is_used_for_reference(dpb.fs[i])))
    {
      remove_frame_from_dpb(i);
      return 1;
    }
  }
  return 0;
}

/*!
 ************************************************************************
 * \brief
 *    Output one picture stored in the DPB.
 ************************************************************************
 */
static void output_one_frame_from_dpb()
{
  int poc, pos;
  //diagnostics
  if (dpb.used_size<1)
  {
    error("Cannot output frame, DPB empty.",150);
  }

  // find smallest POC
  get_smallest_poc(&poc, &pos);

  if(pos==-1)
  {
    error("no frames for output available", 150);
  }

  // call the output function
//  printf ("output frame with frame_num #%d, poc %d (dpb. dpb.size=%d, dpb.used_size=%d)\n", dpb.fs[pos]->frame_num, dpb.fs[pos]->frame->poc, dpb.size, dpb.used_size);

  write_stored_frame(dpb.fs[pos], p_dec);

  if (dpb.last_output_poc >= poc)
  {
    error ("output POC must be in ascending order", 150);
  } 
  dpb.last_output_poc = poc;
  // free frame store and move empty store to end of buffer
  if (!is_used_for_reference(dpb.fs[pos]))
  {
    remove_frame_from_dpb(pos);
  }
}



/*!
 ************************************************************************
 * \brief
 *    All stored picture are output. Should be called to empty the buffer
 ************************************************************************
 */
void flush_dpb()
{
  unsigned i;

  //diagnostics
//  printf("Flush remaining frames from dpb. dpb.size=%d, dpb.used_size=%d\n",dpb.size,dpb.used_size);

  // mark all frames unused
  for (i=0; i<dpb.used_size; i++)
  {
    unmark_for_reference (dpb.fs[i]);
  }

  while (remove_unused_frame_from_dpb()) ;
  
  // output frames in POC order
  while (dpb.used_size)
  {
    output_one_frame_from_dpb();
  }
}

/*!
 ************************************************************************
 * \brief
 *    Extract top field from a frame
 ************************************************************************
 */
void dpb_split_field(FrameStore *fs)
{
  int i;

  fs->top_field    = alloc_storable_picture(TOP_FIELD,    fs->frame->size_x, fs->frame->size_y/2, fs->frame->size_x_cr, fs->frame->size_y_cr/2);
  fs->bottom_field = alloc_storable_picture(BOTTOM_FIELD, fs->frame->size_x, fs->frame->size_y/2, fs->frame->size_x_cr, fs->frame->size_y_cr/2);

  for (i=0; i<fs->frame->size_y/2; i++)
  {
    memcpy(fs->top_field->imgY[i], fs->frame->imgY[i*2], fs->frame->size_x);
  }

  for (i=0; i<fs->frame->size_y_cr/2; i++)
  {
    memcpy(fs->top_field->imgUV[0][i], fs->frame->imgUV[0][i*2], fs->frame->size_x_cr);
    memcpy(fs->top_field->imgUV[1][i], fs->frame->imgUV[1][i*2], fs->frame->size_x_cr);
  }

  for (i=0; i<fs->frame->size_y/2; i++)
  {
    memcpy(fs->bottom_field->imgY[i], fs->frame->imgY[i*2 + 1], fs->frame->size_x);
  }

  for (i=0; i<fs->frame->size_y_cr/2; i++)
  {
    memcpy(fs->bottom_field->imgUV[0][i], fs->frame->imgUV[0][i*2 + 1], fs->frame->size_x_cr);
    memcpy(fs->bottom_field->imgUV[1][i], fs->frame->imgUV[1][i*2 + 1], fs->frame->size_x_cr);
  }

  UnifiedOneForthPix(fs->top_field);
  UnifiedOneForthPix(fs->bottom_field);

  fs->poc = fs->top_field->poc 
          = fs->bottom_field->poc 
          = fs->frame->poc;
  fs->top_field->used_for_reference = fs->bottom_field->used_for_reference 
                                    = fs->frame->used_for_reference;
  fs->top_field->is_long_term = fs->bottom_field->is_long_term 
                              = fs->frame->is_long_term;
  fs->long_term_frame_idx = fs->top_field->long_term_frame_idx 
                          = fs->bottom_field->long_term_frame_idx 
                          = fs->frame->long_term_frame_idx;

  fs->top_field->coded_frame = fs->bottom_field->coded_frame = 1;
  fs->top_field->mb_adaptive_frame_field_flag = fs->bottom_field->mb_adaptive_frame_field_flag
                                              = fs->frame->mb_adaptive_frame_field_flag;

  fs->frame->top_field    = fs->top_field;
  fs->frame->bottom_field = fs->bottom_field;

	//store reference picture index
	 if (!active_sps->frame_mbs_only_flag)
	 {
		 for (i=0;i<listXsize[LIST_1];i++)
		 {
			 fs->top_field->ref_pic_num[LIST_1][2*i]=fs->frame->ref_pic_num[LIST_1][i];
			 fs->top_field->ref_pic_num[LIST_1][2*i + 1]=fs->frame->ref_pic_num[LIST_1][i] + 1;
			 fs->bottom_field->ref_pic_num[LIST_1][2*i]=fs->frame->ref_pic_num[LIST_1][i] + 1;
			 fs->bottom_field->ref_pic_num[LIST_1][2*i+1]=fs->frame->ref_pic_num[LIST_1][i] ;
		 }
		 
		 for (i=0;i<listXsize[LIST_0];i++)
		 {
			 fs->top_field->ref_pic_num[LIST_0][2*i]=fs->frame->ref_pic_num[LIST_0][i];
			 fs->top_field->ref_pic_num[LIST_0][2*i + 1]=fs->frame->ref_pic_num[LIST_0][i] + 1;
			 fs->bottom_field->ref_pic_num[LIST_0][2*i]=fs->frame->ref_pic_num[LIST_0][i] + 1;
			 fs->bottom_field->ref_pic_num[LIST_0][2*i+1]=fs->frame->ref_pic_num[LIST_0][i] ;
		 }
	 }
}


/*!
 ************************************************************************
 * \brief
 *    Generate a frame from top and bottom fields
 ************************************************************************
 */
void dpb_combine_field(FrameStore *fs)
{
  int i,j;

  fs->frame = alloc_storable_picture(FRAME, fs->top_field->size_x, fs->top_field->size_y*2, fs->top_field->size_x_cr, fs->top_field->size_y_cr*2);

  for (i=0; i<fs->top_field->size_y; i++)
  {
    memcpy(fs->frame->imgY[i*2],     fs->top_field->imgY[i]   , fs->top_field->size_x);     // top field
    memcpy(fs->frame->imgY[i*2 + 1], fs->bottom_field->imgY[i], fs->bottom_field->size_x); // bottom field
  }

  for (i=0; i<fs->top_field->size_y_cr; i++)
  {
    memcpy(fs->frame->imgUV[0][i*2],     fs->top_field->imgUV[0][i],    fs->top_field->size_x_cr);
    memcpy(fs->frame->imgUV[0][i*2 + 1], fs->bottom_field->imgUV[0][i], fs->bottom_field->size_x_cr);
    memcpy(fs->frame->imgUV[1][i*2],     fs->top_field->imgUV[1][i],    fs->top_field->size_x_cr);
    memcpy(fs->frame->imgUV[1][i*2 + 1], fs->bottom_field->imgUV[1][i], fs->bottom_field->size_x_cr);
  }
  
  UnifiedOneForthPix(fs->frame);
  
  fs->poc=fs->frame->poc = min (fs->top_field->poc, fs->bottom_field->poc);
  fs->frame->used_for_reference = (fs->top_field->used_for_reference && fs->bottom_field->used_for_reference );
  fs->frame->is_long_term = (fs->top_field->is_long_term && fs->bottom_field->is_long_term );

  if (fs->frame->is_long_term) 
    fs->frame->long_term_frame_idx = fs->long_term_frame_idx;

  fs->frame->top_field    = fs->top_field;
  fs->frame->bottom_field = fs->bottom_field;
  
  fs->top_field->frame = fs->bottom_field->frame = fs->frame;

	//combine field for frame
	for (i=0;i<(listXsize[LIST_1]+1)/2;i++)
  {
    fs->frame->ref_pic_num[LIST_1][i]=(fs->top_field->ref_pic_num[LIST_1][2*i]/2)*2;
  }

  for (i=0;i<(listXsize[LIST_0]+1)/2;i++)
  {
    fs->frame->ref_pic_num[LIST_0][i]=(fs->top_field->ref_pic_num[LIST_0][2*i]/2)*2;
  }

   //! Use inference flag to remap mvs/references 
  
    //! Generate Frame parameters from field information.
	for (i=0 ; i<fs->top_field->size_x/4 ; i++)
	{
		for (j=0 ; j<fs->top_field->size_y/2 ; j++)
		{        
			fs->frame->mv[LIST_0][i][j][0] = fs->top_field->mv[LIST_0][i][j/2][0];
			fs->frame->mv[LIST_0][i][j][1] = fs->top_field->mv[LIST_0][i][j/2][1] ;
			fs->frame->mv[LIST_1][i][j][0] = fs->top_field->mv[LIST_1][i][j/2][0];
			fs->frame->mv[LIST_1][i][j][1] = fs->top_field->mv[LIST_1][i][j/2][1] ; 
			
			fs->frame->ref_idx[LIST_0][i][j]  = fs->top_field->ref_idx[LIST_0][i][j/2];
			fs->frame->ref_idx[LIST_1][i][j]  = fs->top_field->ref_idx[LIST_1][i][j/2];
		}     
	}
	
	for (i=0 ; i<fs->top_field->size_x/4 ; i++)
	{
		for (j=0 ; j<fs->top_field->size_y/2 ; j++)
		{
			//! Use inference flag to remap mvs/references
			fs->frame->mv[LIST_0][i][j][0]=fs->frame->mv[LIST_0][RSD(i)][RSD(j)][0];
			fs->frame->mv[LIST_0][i][j][1]=fs->frame->mv[LIST_0][RSD(i)][RSD(j)][1];
			fs->frame->mv[LIST_1][i][j][0]=fs->frame->mv[LIST_1][RSD(i)][RSD(j)][0];
			fs->frame->mv[LIST_1][i][j][1]=fs->frame->mv[LIST_1][RSD(i)][RSD(j)][1];        
			
			fs->frame->ref_idx[LIST_0][i][j]=fs->frame->ref_idx[LIST_0][RSD(i)][RSD(j)] ;     
			fs->frame->ref_idx[LIST_1][i][j]=fs->frame->ref_idx[LIST_1][RSD(i)][RSD(j)] ;     
			
			if(input->successive_Bframe!=0&&input->direct_type == DIR_SPATIAL)
			{
				moving_block_frm[j][i]= 
					!(((fs->frame->ref_idx[LIST_0][i][j] == 0) && 
					(abs(fs->frame->mv[LIST_0][i][j][0])>>1 == 0) && 
					(abs(fs->frame->mv[LIST_0][i][j][1])>>1 == 0)) || 
					((fs->frame->ref_idx[LIST_0][i][j] == -1) && 
					(fs->frame->ref_idx[LIST_1][i][j] == 0) && 
					(abs(fs->frame->mv[LIST_1][i][j][0])>>1 == 0) && 
					(abs(fs->frame->mv[LIST_1][i][j][1])>>1 == 0)));
			}
		}      
	}
	
	//scaling of mvs/references needs to be done separately 
	for (i=0 ; i<fs->top_field->size_x/4 ; i++)
	{
		for (j=0 ; j<fs->top_field->size_y/2 ; j++)
		{                
			fs->frame->mv[LIST_0][i][j][1] *= 2;        
			fs->frame->mv[LIST_1][i][j][1] *= 2;
			if (fs->frame->ref_idx[LIST_0][i][j] != -1)
        fs->frame->ref_idx[LIST_0][i][j] >>= 1;
			if (fs->frame->ref_idx[LIST_1][i][j] != -1)
        fs->frame->ref_idx[LIST_1][i][j] >>= 1;
		}     
	}
	
	
  if (!active_sps->frame_mbs_only_flag || active_sps->direct_8x8_inference_flag)      
  {        
    for (i=0 ; i<fs->top_field->size_x/4 ; i++)
    {
      for (j=0 ; j<fs->top_field->size_y/4 ; j++)
      {        
        fs->top_field->mv[LIST_0][i][j][0]=fs->top_field->mv[LIST_0][RSD(i)][RSD(j)][0];
        fs->top_field->mv[LIST_0][i][j][1]=fs->top_field->mv[LIST_0][RSD(i)][RSD(j)][1];
        fs->top_field->mv[LIST_1][i][j][0]=fs->top_field->mv[LIST_1][RSD(i)][RSD(j)][0];
        fs->top_field->mv[LIST_1][i][j][1]=fs->top_field->mv[LIST_1][RSD(i)][RSD(j)][1];
        
        fs->bottom_field->mv[LIST_0][i][j][0]=fs->bottom_field->mv[LIST_0][RSD(i)][RSD(j)][0];
        fs->bottom_field->mv[LIST_0][i][j][1]=fs->bottom_field->mv[LIST_0][RSD(i)][RSD(j)][1];
        fs->bottom_field->mv[LIST_1][i][j][0]=fs->bottom_field->mv[LIST_1][RSD(i)][RSD(j)][0];
        fs->bottom_field->mv[LIST_1][i][j][1]=fs->bottom_field->mv[LIST_1][RSD(i)][RSD(j)][1];
        
        fs->top_field->ref_idx[LIST_0][i][j]=fs->top_field->ref_idx[LIST_0][RSD(i)][RSD(j)] ;     
        fs->top_field->ref_idx[LIST_1][i][j]=fs->top_field->ref_idx[LIST_1][RSD(i)][RSD(j)] ;     
        
        fs->bottom_field->ref_idx[LIST_0][i][j]=fs->bottom_field->ref_idx[LIST_0][RSD(i)][RSD(j)] ;     
        fs->bottom_field->ref_idx[LIST_1][i][j]=fs->bottom_field->ref_idx[LIST_1][RSD(i)][RSD(j)] ;     
				if(input->successive_Bframe!=0&&input->direct_type == DIR_SPATIAL)
				{
					
				moving_block_top[j][i] =  
					!(((fs->top_field->ref_idx[LIST_0][i][j] == 0) && 
					(abs(fs->top_field->mv[LIST_0][i][j][0])>>1 == 0) && 
					(abs(fs->top_field->mv[LIST_0][i][j][1])>>1 == 0)) || 
					((fs->top_field->ref_idx[LIST_0][i][j] == -1) && 
					(fs->top_field->ref_idx[LIST_1][i][j] == 0) && 
					(abs(fs->top_field->mv[LIST_1][i][j][0])>>1 == 0) && 
					(abs(fs->top_field->mv[LIST_1][i][j][1])>>1 == 0)));
				
				moving_block_bot[j][i]= 
					!(((fs->bottom_field->ref_idx[LIST_0][i][j] == 0) && 
					(abs(fs->bottom_field->mv[LIST_0][i][j][0])>>1 == 0) && 
					(abs(fs->bottom_field->mv[LIST_0][i][j][1])>>1 == 0)) || 
					((fs->bottom_field->ref_idx[LIST_0][i][j] == -1) && 
					(fs->bottom_field->ref_idx[LIST_1][i][j] == 0) && 
					(abs(fs->bottom_field->mv[LIST_1][i][j][0])>>1 == 0) && 
					(abs(fs->bottom_field->mv[LIST_1][i][j][1])>>1 == 0)));
				}
      }
    }    		    
  } 
}


/*!
 ************************************************************************
 * \brief
 *    Allocate memory for buffering of reference picture reordering commands
 ************************************************************************
 */
void alloc_ref_pic_list_reordering_buffer(Slice *currSlice)
{
  int size = img->num_ref_idx_l0_active;

  if (img->type!=I_SLICE && img->type!=SI_SLICE)
  {
    if ((currSlice->remapping_of_pic_nums_idc_l0 = calloc(size,sizeof(int)))==NULL) no_mem_exit("alloc_ref_pic_list_reordering_buffer: remapping_of_pic_nums_idc_l0");
    if ((currSlice->abs_diff_pic_num_minus1_l0 = calloc(size,sizeof(int)))==NULL) no_mem_exit("alloc_ref_pic_list_reordering_buffer: abs_diff_pic_num_minus1_l0");
    if ((currSlice->long_term_pic_idx_l0 = calloc(size,sizeof(int)))==NULL) no_mem_exit("alloc_ref_pic_list_reordering_buffer: long_term_pic_idx_l0");
  }
  else
  {
    currSlice->remapping_of_pic_nums_idc_l0 = NULL;
    currSlice->abs_diff_pic_num_minus1_l0 = NULL;
    currSlice->long_term_pic_idx_l0 = NULL;
  }
  
  size = img->num_ref_idx_l1_active;

  if (img->type!=B_SLICE)
  {
    if ((currSlice->remapping_of_pic_nums_idc_l1 = calloc(size,sizeof(int)))==NULL) no_mem_exit("alloc_ref_pic_list_reordering_buffer: remapping_of_pic_nums_idc_l1");
    if ((currSlice->abs_diff_pic_num_minus1_l1 = calloc(size,sizeof(int)))==NULL) no_mem_exit("alloc_ref_pic_list_reordering_buffer: abs_diff_pic_num_minus1_l1");
    if ((currSlice->long_term_pic_idx_l1 = calloc(size,sizeof(int)))==NULL) no_mem_exit("alloc_ref_pic_list_reordering_buffer: long_term_pic_idx_l1");
  }
  else
  {
    currSlice->remapping_of_pic_nums_idc_l1 = NULL;
    currSlice->abs_diff_pic_num_minus1_l1 = NULL;
    currSlice->long_term_pic_idx_l1 = NULL;
  }
}


/*!
 ************************************************************************
 * \brief
 *    Free memory for buffering of reference picture reordering commands
 ************************************************************************
 */
void free_ref_pic_list_reordering_buffer(Slice *currSlice)
{

  if (currSlice->remapping_of_pic_nums_idc_l0) 
    free(currSlice->remapping_of_pic_nums_idc_l0);
  if (currSlice->abs_diff_pic_num_minus1_l0)
    free(currSlice->abs_diff_pic_num_minus1_l0);
  if (currSlice->long_term_pic_idx_l0)
    free(currSlice->long_term_pic_idx_l0);

  currSlice->remapping_of_pic_nums_idc_l0 = NULL;
  currSlice->abs_diff_pic_num_minus1_l0 = NULL;
  currSlice->long_term_pic_idx_l0 = NULL;
  
  if (currSlice->remapping_of_pic_nums_idc_l1)
    free(currSlice->remapping_of_pic_nums_idc_l1);
  if (currSlice->abs_diff_pic_num_minus1_l1)
    free(currSlice->abs_diff_pic_num_minus1_l1);
  if (currSlice->long_term_pic_idx_l1)
    free(currSlice->long_term_pic_idx_l1);
  
  currSlice->remapping_of_pic_nums_idc_l1 = NULL;
  currSlice->abs_diff_pic_num_minus1_l1 = NULL;
  currSlice->long_term_pic_idx_l1 = NULL;
}


//-------- end new Buffer code -----------------------------

/*!
 ************************************************************************
 * \brief
 *      Tian Dong, June 13, 2002
 *      If a PN gap is found, try to fill the gap
 * \param img
 *      
 ************************************************************************
 */
#if 0
void fill_PN_gap(ImageParameters *img)
{
  int pn_expected, pn_new;

  if (fb==0 || fb!=frm) return;

  if ( img->type == B_SLICE ) return;

  pn_expected = (fb->picbuf_short[0]->picID+1) % (img->buf_cycle);

  while ( pn_new != pn_expected)
  {
    fb = frm;
    add_frame(pn_expected, 0);

    fb = fld;
    add_frame(pn_expected, 0);
    add_frame(pn_expected, 0);

    pn_expected = (fb->picbuf_short[0]->picID + 1) % (img->buf_cycle);
    img->number++;
  }
  fb = frm;
}

#endif
