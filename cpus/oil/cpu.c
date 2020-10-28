/* cpu.c OIL cpu-description file */
/* (c) in 2007 by Volker Barthelmann */

#include "vasm.h"

char *cpu_copyright="vasm OIL cpu backend 1.0i (c) in 2007-2008 Volker Barthelmann";
char *cpuname="OIL";

mnemonic mnemonics[]={
#include "opcodes.h"
};

int mnemonic_cnt=sizeof(mnemonics)/sizeof(mnemonics[0]);

int bitsperbyte=8;
int bytespertaddr=4;

int parse_operand(char *p,int len,operand *op,int requires)
{
  char *s;
  p=skip(p);
  op->value=0;
  if(len==0){
    op->type=OP_NONE;
    return 1;
  }
  if(requires==OP_NONE&&len!=0)
    return 0;
  if(requires==OP_ADDR_IND){
    if(*p=='*'){
      op->type=OP_ADDR_IND;
      p=skip(p+1);
    }else
      op->type=OP_ADDR;
  }else
    op->type=requires;
  s=p;
  op->value=parse_expr(&s);
  if(p==s)
    return 0;

  return 1;
}


/* Convert an instruction into a DATA atom including relocations,
   if necessary. */
dblock *eval_instruction(instruction *ip,section *sec,taddr pc)
{
  unsigned short ind=0,opcode,idx=0,ed=0,uefs=0,hival;
  taddr value=0,addr=0,val;
  unsigned short byte2,byte3,wpos,numbits,inibits,inihi,inilo,histruct,lostruct,dev;
  int structp=0;
  int i,c;
  operand *oneop;
  
  byte2=byte3=wpos=numbits=inibits=0;
  
  c=ip->code;
  dblock *db=new_dblock();

  

  for (i=0;i<MAX_OPERANDS;i++){  /*for each operand: */
    int constant;
    oneop=ip->op[i];
    if (oneop != NULL){
      if(oneop->type!=OP_NONE)
	constant=eval_expr(oneop->value,&val,sec,pc);
      switch (oneop->type){
      case OP_ADDR_IND:
	ind=1;
	/* fall-through */
      case OP_ADDR:
	addr=val;
	if(!constant){
	  symbol *base;
	  int btype;
	  btype=find_base(oneop->value,&base,sec,pc);
	  if (btype==BASE_ILLEGAL){
	    general_error(38);
	  } else {
	    add_nreloc(&db->relocs,base,val,
                       (btype==BASE_PCREL)?REL_PC:REL_ABS,32,32);
          }
	}
	break;
      case OP_INDEX:
	idx=val;
	if(!constant){
	  cpu_error(2,"idx");
	}else if (idx>3){
	  cpu_error(1,"idx");
	}
	break;
      case OP_DELEXT:
	ed=val;
	if(!constant){
	  cpu_error(2,"ed");
	}else if (ed>3){
	  cpu_error(1,"ed");
	}
	break;
      case OP_UE:
	uefs=val;
	if(!constant){
	  cpu_error(2,"uefs");
	}else if (uefs>3){
	  cpu_error(1,"uefs");
	}
	break;
      case OP_DEVICE:
	dev=val;
	if(!constant)
	  cpu_error(2,"device");
	else if(dev>15)
	  cpu_error(1,"device");
	ed=dev>>2;
	uefs=dev&3;
	break;
      case OP_VALUE:
	value=val;
	if(!constant){
	  cpu_error(2,"value");
	}else if (value>262143){
	  cpu_error(1,"value");
	}
	break;
      case OP_WPOS:
	wpos=val;
	if(!constant){
	  cpu_error(2,"WordPosition");
	}else if (wpos>255){
	  cpu_error(1,"WordPosition");
	}
	break;
      case OP_NUMBITS:
	numbits=val;
	if(!constant){
	  cpu_error(2,"NumberBits");
	}else if (numbits>31){
	  cpu_error(1,"NumberBits");
	}
	break;
      case OP_INIBITS:
	inibits=val;
	if(!constant){
	  cpu_error(2,"InitialBits");
	}else if (inibits>31){
	  cpu_error(1,"InitialBits");
	}
	break;
      case OP_STRUCTP:
	structp=val;
	if(!constant){
	  cpu_error(2,"StructPointer");
	}else if (structp>16383){
	  cpu_error(1,"StructPointer");
	}
	break;
      } /*end switch*/
    }
  } /*end for*/
  if (mnemonics[c].ext.opcode==255){		/*  isStruct  */
    
    byte2=numbits<<3;inihi=inibits&28;	
    byte2=byte2|(inihi>>2);inilo=inibits&3;	
    byte3=inilo<<6;lostruct=structp&255;	
    histruct=structp>>8;	
    byte3|=histruct;	
    
    db->size=4;	
    db->data=mymalloc(4);	
    
    db->data[0]=wpos;	
    db->data[1]=byte2;	
    db->data[2]=byte3;	
    db->data[3]=lostruct;
  }else { /* normal */
    
    db->size=8;
    db->data=mymalloc(8);
    
    hival=(value>>16)&3;
    
    if (mnemonics[c].ext.encoding&EN_JIF){     /* isJIF */
      int jif_en;
      jif_en=(mnemonics[c].ext.encoding&15);
      byte2|=idx;byte2=(byte2<<4)|jif_en;
      byte2=(byte2<<2)|hival;
      
    } else {
      
      byte2|=idx; byte2=(byte2<<2)|ed;     /*order important!*/
      byte2=(byte2<<2)|uefs; byte2=(byte2<<2)|hival; 
    }
    
    
    db->data[0]=(ind<<7)|mnemonics[c].ext.opcode;
    db->data[1]=byte2;
    db->data[2]=value>>8;
    db->data[3]=value;
    db->data[7]=addr;
    db->data[6]=(addr>>=8);
    db->data[5]=(addr>>=8);
    db->data[4]=addr>>8;
    
  }
    
  return db;
}


/* Create a dblock (with relocs, if necessary) for size bits of data. */
dblock *eval_data(operand *op,size_t bitsize,section *sec,taddr pc)
{
  dblock *new=new_dblock();
  taddr val=0;
  new->size=(bitsize+7)/8;
  new->data=mymalloc(new->size);
  if(op->type!=OP_ADDR)
    ierror(0);
  if(bitsize!=8&&bitsize!=16&&bitsize!=32)
    cpu_error(4);

  if(!eval_expr(op->value,&val,sec,pc)){
    symbol *base;
    int btype;
    btype=find_base(op->value,&base,sec,pc);
    if (btype==BASE_ILLEGAL){
      general_error(38);
    } else {
      add_nreloc(&new->relocs,base,val,
                 (btype==BASE_PCREL)?REL_PC:REL_ABS,32,0);
    }
  }

  if(bitsize==32){
    new->data[0]=val>>24;
    new->data[1]=val>>16;    
    new->data[2]=val>>8;
    new->data[3]=val;        
  }else if(bitsize==16){
    new->data[0]=val>>8;
    new->data[1]=val;    
  }else
    new->data[0]=val;
  return new;
}                                     


/* Calculate the size of the current instruction; must be identical
   to the data created by eval_instruction. */
size_t instruction_size(instruction *ip,section *sec,taddr pc)
{
 int c;
 c=ip->code;
 return (mnemonics[c].ext.opcode==255) ? 4 : 8;

}

operand *new_operand()
{
  operand *new=mymalloc(sizeof(*new));
  new->type=-1;
  return new;
}

/* return true, if initialization was successfull */
int init_cpu()
{
  return 1;
}

/* return true, if the passed argument is understood */
int cpu_args(char *p)
{
  return 0;
}

/* parse cpu-specific directives; return pointer to end of
   cpu-specific text */
char *parse_cpu_special(char *s)
{
  char *old=s,esc;
  s=skip(s);
  if(*s=='.') s++;
  if(!strncmp(s,"textelem",8)){
    int cnt=0,i,sz;
    char *p,*dp,*lab="missinglabel";
    dblock *db=new_dblock();
    s=skip(s+8);
    if(ISIDSTART(*s)){
      char *e=s+1;
      while(ISIDCHAR(*e))
	e++;
      lab=cnvstr(s,e-s);
      s=skip(e);
    }else
      cpu_error(7);
    s=skip(s);
    if(*s==',')
      s++;
    else
      cpu_error(6);
    s=skip(s);
    if(*s!='\"')
      cpu_error(5);
    else
      s++;
    while(*s!='\"'&&*s!=0&&*s!='\n'){
      if(*s=='\\'){
	char *n;
	n=escape(s,&esc);
	*s=esc;
	memmove(s+1,n,strlen(n));
	s[strlen(s)-1]=0;
	s++;
      }else{
	s++;
      }
      cnt++;
    }
    p=s-1;
    if(*s!='\"')
      cpu_error(5);
    else
      s++;
    s=skip(s);
    db->size=(cnt+1)/2*4;
    sz=(db->size+4)/4;
    db->data=mymalloc(db->size);
    dp=db->data;
    i=cnt;
    if(i&1){
      *dp++=0;
      *dp++=*p;
      *dp++=0;
      *dp++=' ';
      i--;
      p--;
    }    
    while(i>0){
      *dp++=0;
      *dp++=*(p-1);
      *dp++=0;
      *dp++=*p;
      p-=2;
      i-=2;
    }
    add_atom(0,new_data_atom(db,2));
    add_atom(0,new_label_atom(new_labsym(0,lab)));
    db=new_dblock();
    db->size=4;
    db->data=dp=mymalloc(db->size);
    *dp++=(sz)>>24;
    *dp++=(sz)>>16;
    *dp++=(sz)>>8;
    *dp++=(sz);
    add_atom(0,new_data_atom(db,2));
    return s;
  }
	
  return old;
}
