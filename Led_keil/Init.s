  PRESERVE8
  AREA RESET,CODE,READONLY 
  ENTRY 
  ldr r13, =0x1000 
  IMPORT ledMain 
  b ledMain 
  END
