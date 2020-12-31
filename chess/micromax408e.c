/***************************************************************************/
/*                               micro-Max,                                */
/* A chess program orig. <  2 KByte (of non-blank source), by H.G. Muller  */
/***************************************************************************/
/* version 4.8.e  RasPi   features:                                        */

/* http://home.hccnet.nl/h.g.muller/umax4_8.c                              */
/* version 4.8 (1953 characters) features:                                 */
/* - recursive negamax search                                              */
/* - all-capture MVV/LVA quiescence search                                 */
/* - (internal) iterative deepening                                        */
/* - best-move-first 'sorting'                                             */
/* - a hash table storing score and best move                              */
/* - futility pruning                                                      */
/* - king safety through magnetic, frozen king in middle-game              */
/* - R=2 null-move pruning                                                 */
/* - keep hash and repetition-draw detection                               */
/* - better defense against passers through gradual promotion              */
/* - extend check evasions in inner nodes                                  */
/* - reduction of all non-Pawn, non-capture moves except hash move (LMR)   */
/* - full FIDE rules (expt under-promotion) and move-legality checking     */

// Code für Raspberry Pi

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#define K(A,B) *(int*)(T+A+(B&8)+S*(B&7))

#define HASHSIZE (1<<12) //  wegen RAM, für PC: (1<<24) // <<<<<<  für Arduino Mega verkleinern auf 1<<8 !

struct HTab {
          int  Key,
               Score;
          int  From,
               To,
               Draft;
        } HashTab[HASHSIZE];           /* hash table, HTsize entries*/


#define MAXNODES 800000   //  wegen Zugdauer; für PC: x10 = 800000 = 8e5   // <<<<<<  für Mega verkleinern auf 10000!

int  K,
     RootEval,
     R,
     HashKeyLo,
     HashKeyHi;

int32_t    N,
          INF=80000;                         /* INF=80000: "infinity e" */ ;

int
    M=136,                           /* M=0x88   board system    */
    S=128,                           /* dummy square 0x80, highest valid square =127    */
    Side=16;                         // 16=Weiss, 8=Schwarz; Side^=24 wechselt hin unnd her

signed char  L,
     pval[]={0,2,2,7,-1,8,12,23},                      /* relative piece values    */
     vector[]={-16,-15,-17,0,1,16,0,1,16,15,17,0,14,18,31,33,0, /* step-vector lists */
          7,-1,11,6,8,3,6},                          /* 1st dir. in vector[] per piece*/

     bsetup[]={6,3,5,7,4,5,3,6},                         /* initial piece setup      */
     board[129],                                        /* board: half of 16x8+dummy*/
     T[1035];                                        /* hash translation table   */

signed char  psymbol[]= ".?+nkbrq?*?NKBRQ";

int  mfrom, mto;    // current ply from - to
int  Rootep,           // e.PieceType. square
     RemP;           // remove piece




/* recursive minimax search, Side=moving side, n=depth*/

int  Minimax(int32_t  Alpha, int32_t  Beta, int32_t  Eval, int  epSqr, int  prev, int32_t   Depth)
                       /* (Alpha,Beta)=window, Eval, EnPass_sqr.*/
                       /* prev=prev.dest; HashKeyLo,HashKeyHi=hashkeys; return e*/
{
   int  j,
        StepVec,
        m,
        v,
        d,
        h,
        i,
        SkipSqr,
        G,
        V,
        P,
        f=HashKeyLo,
        g=HashKeyHi,
        C,
        s;
   signed char  t,
        PieceType,
        Piece,
        x,
        y,
        X,
        Y,
        H,
        B;

   struct HTab *a = HashTab + (HashKeyLo + Side * epSqr & HASHSIZE-1);   /* lookup pos. in hash table*/

   char sbuf[50];



   Alpha--;                                          /* adj. window: delay bonus */
   Side^=24;                                        /* change sides             */
   d=a->Draft;
   m=a->Score;
   X=a->From;
   Y=a->To;                  /* resume at stored depth   */

   if(a->Key-HashKeyHi|prev|                                  /* miss: other pos. or empty*/
      !(m<=Alpha | X&8&&m>=Beta | X&S))                   /*   or window incompatible */
      { d=Y=0; }                                /* start iter. from scratch */

   X&=~M;                                        /* start at best-move hint  */

   while( d++ <  Depth || d<3                   /* iterative deepening loop */
     || prev&K == INF
       && ( N<60000 & d<98                        /* root: deepen upto time   */
          || (K=X, L=Y&~M, d=3)
       )
     )                                          /* time's up: go do best    */
   {
      x=B=X;                                       /* start scan at prev. best */
      h=Y&S;                                       /* request try noncastl. 1st*/
      P=d<3 ? INF : Minimax(-Beta,1-Beta,-Eval,S,0,d-3);               /* Search null move         */
      m = (-P<Beta | R>35) ? ( d>2 ? -INF : Eval ) : -P;            /* Prune or stand-pat       */
      N++;                                         /* node count (for timing)  */
      do
      {
         Piece=board[x];                                   /* scan board looking for   */
         if(Piece & Side)                                  /*  own piece (inefficient!)*/
         {
            StepVec = PieceType = Piece&7;                               /* PieceType = piece type (set StepVec>0) */
            j = vector[PieceType+16];                                 /* first step vector f.piece*/
            while(StepVec = PieceType>2 & StepVec<0 ? -StepVec : -vector[++j] )       /* loop over directions vector[] */
            {
labelA:                                        /* resume normal after best */
               y=x;                            /* (x,y)=move         */
               SkipSqr=G=S;                          /* (SkipSqr,G)=castl.R      */

               do
               {                                       /* y traverses ray, or:     */
                  H=y=h?Y^h:y+StepVec;                           /* sneak in prev. best move */

                  if(y&M)break;                            /* board edge hit           */

                  m= epSqr-S&board[epSqr]&&y-epSqr<2&epSqr-y<2?INF:m;      /* bad castling             */

                  if(PieceType<3&y==epSqr)H^=16;                           /* shift capt.sqr. H if Eval.PieceType.*/

                  t=board[H];

                  if(t&Side|PieceType<3&!(y-x&7)-!t)break;            /* capt. own, bad pawn mode */
                  i=37*pval[t&7]+(t&192);                     /* value of capt. piece t   */
                  m=i<0?INF:m;                                  /* K capture                */

                  if(m>=Beta&d>1) goto labelC;                     /* abort on fail high       */

                  v=d-1?Eval:i-PieceType;                             /* MVV/LVA scoring          */

                  if(d-!t>1)                               /* remaining depth          */
                  {
                     v=PieceType<6?board[x+8]-board[y+8]:0;                  /* center positional pts.   */
                     board[G]=board[H]=board[x]=0;board[y]=Piece|32;             /* do move, set non-virgin  */
                     if(!(G&M))board[SkipSqr]=Side+6,v+=50;               /* castling: put R & Eval  */
                     v-=PieceType-4|R>29?0:20;                              /* penalize mid-game K move */

                     if(PieceType<3)                                        /* pawns:                   */
                     {
                        v-=9*((x-2&M||board[x-2]-Piece)+              /* structure, undefended    */
                               (x+2&M||board[x+2]-Piece)-1            /*        squares plus bias */
                              +(board[x^16]==Side+36))                 /* kling to non-virgin King */
                              -(R>>2);                                 /* end-game Pawn-push bonus */
                         V=y+StepVec+1&S?647-PieceType:2*(Piece&y+16&32);           /* promotion or 6/7th bonus */
                         board[y]+=V;
                         i+=V;                                         /* change piece, add Eval  */
                     }

                     v+= Eval+i;
                     V=m>Alpha ? m : Alpha;                           /* new eval and alpha       */
                     HashKeyLo+=K(y+0,board[y])-K(x+0,Piece)-K(H+0,t);
                     HashKeyHi+=K(y+8,board[y])-K(x+8,Piece)-K(H+8,t)+G -S;  /* update hash key          */
                     C=d-1-(d>5&PieceType>2&!t&!h);
                     C=R>29|d<3|P-INF?C:d;                     /* extend 1 ply if in check */
                     do {
                        s=C>2|v>V?-Minimax(-Beta,-V,-v,         /* recursive eval. of reply */
                                            SkipSqr,0,C):v;        /* or fail low if futile    */
                     } while( s>Alpha & ++C<d );

                     v=s;
                     if(prev&&K-INF&&v+INF&&x==K&y==L)              /* move pending & in root:  */
                     {
                        RootEval=-Eval-i; Rootep=SkipSqr;                            /*   exit if legal & found  */
                        a->Draft=99;a->Score=0;                        /* lock game in hash as draw*/
                        R+=i>>7;
                        return Beta;                            /* captured non-P material  */
                     }
                     HashKeyLo=f;
                     HashKeyHi=g;                                    /* restore hash key         */
                     board[G]=Side+6;
                     board[SkipSqr]=board[y]=0;
                     board[x]=Piece;
                     board[H]=t;                             /* undo move,G can be dummy */
                  }
                  if(v>m)                                  /* new best, update max,best*/
                  {
                     m=v,X=x,Y=y|S&SkipSqr;                      /* mark double move with S  */
                  }
                  if(h)
                  {
                     h=0;
                     goto labelA;                           /* redo after doing old best*/
                  }
                  if (
                    x+StepVec-y || Piece&32|                             /* not 1st step,moved before*/
                    PieceType>2 & (PieceType-4|j-7||                     /* no P & no lateral K move,*/
                      board[G=x+3^StepVec>>1&7]-Side-6            /* no virgin R in corner G, */
                      || board[G^1] | board[G^2] )          /* no 2 empty sq. next to R */
                    )
                  {
                     t+=PieceType<5;
                  }                                        /* fake capt. for nonsliding*/
                  else SkipSqr=y;                                /* enable e.PieceType.              */

               } while(!t);                               /* if not capt. continue ray*/

            }
         }  // (Piece & Side)

      } while( (x=x+9&~M)-B);                       /* next sqr. of board, wrap */

labelC:
      if (m>INF-M || m<M-INF) d=98;                       /* mate holds to any depth  */
      m= m+INF || P==INF ? m : 0;                         /* best loses K: (stale)mate*/

      if(a->Draft<99) {                                /* protect game history     */
         a->Key=HashKeyHi;
         a->Score=m;
         a->Draft=d;                       /* always store in hash tab */
         a->From=X|8*(m>Alpha)|S*(m<Beta);
         a->To=Y;                       /* move, type (bound/exact),*/
      }
      /* uncomment for Kibitz */
      //if(prev) sprintf(sbuf, "%2d ply, %9d searched, score=%6d by %c%c%c%c\n",
      //     d-1, N-S, m, 'a'+(X&7),'8'-(X>>4),'a'+(Y&7),'8'-(Y>>4&7));

       if(prev  && X!=Y) {
         sprintf(sbuf, "\n%2d ply, searched: %9d ", d-1, N-S );
         printf(sbuf);
       }
       else
       if( ((N-S)%10000)<1) { printf(".");; fflush(stdout); }


   }  // while (iterative deepening loop)

   Side^=24;                                        /* change sides back        */
   mfrom=K; mto=L;
   return m+= m<Eval;                                  /* delayed-loss bonus       */
}





void chess()
{
   int32_t       e;
   int16_t       oldto, oldEPSQ;
   char          sbuf[50], sbuf2[50];
   int16_t       cstring[20], *ptr;
   signed char   oboard[129], spiece;


   K=8;
   while(K--)
   {
      board[K]=(board[K+112]=bsetup[K]+8)+8;
      board[K+16]=18;
      board[K+96]=9;                               /* initial board setup*/
      L=8;
      while(L--)board[16*L+K+8]=(K-4)*(K-4)+(L-3.5)*(L-3.5);     /* center-pts table   */
   }                                                             /*(in unused half board[])*/
   N=1035;
   while(N-->M)T[N]=rand()>>9;

                                                                /* play loop          */
   while(1)
   {
     N=-1;

     printf("\n");

     sprintf(sbuf,"     A B C D E SkipSqr G H \n     --------------- \n"); printf(sbuf);
     while(++N<121) {                                            /* print board */
         if(N&8 && (N+7!=0) ) {sprintf(sbuf,"%3d \n", 1+((120-N)>>4)); printf(sbuf); N+=7; }
         else {
           if(N%8==0) {sprintf(sbuf, "%3d ", 1+((120-N)>>4)); printf(sbuf); }
         sprintf(sbuf," %c", psymbol[board[N]&15]);         printf(sbuf);
         }
     }
     sprintf(sbuf,"     --------------- \n     A B C D E SkipSqr G H "); printf(sbuf);

     if(Side==16) sprintf(sbuf,"\n>  WHITE: ");  else sprintf(sbuf,"\n>   BLACK:  ");
     printf(sbuf);

    
     ptr=cstring;
     while( ( *ptr++ = getchar() ) >10 ) ;
    
     if(cstring[0]=='Q') return;
     if(cstring[0]==10)  cstring[0]=0;


     K=INF;

     if(cstring[0]!=0) {                                   /* parse entered move */
       K= cstring[0]-16*cstring[1]+799;
       L= cstring[2]-16*cstring[3]+799;
     }
    
     /*
     printf("\n DEBUG cstring : "); printf(cstring);
     sprintf(sbuf,"\n DEBUG K: %d  \n DEBUG L: %d \n",  K, L);
     printf(sbuf);
     */
     memcpy(oboard, board, sizeof(board));
     oldto=mto;
     oldEPSQ=Rootep;

     e=Minimax(-INF, INF, RootEval, Rootep, 1, 3);                              /* think or check & do*/

     if(e!=15) {
        RemP=S;
        if(oboard[mto])   RemP=mto;
        if(mto==oldEPSQ)  RemP=oldto;

        spiece=psymbol[board[mto]&15];
        if(spiece=='*' || spiece=='+') spiece=' ';
        sprintf(sbuf,"\n\nmoved: >> %c %c%c", spiece,'a'+(mfrom&7),'8'-(mfrom>>4) );

        if(oboard[mto]) strcat(sbuf," X ");
        else strcat(sbuf,"-");

        sprintf(sbuf2,"%c%c ", 'a'+(mto&7),'8'-(mto>>4&7));
        strcat(sbuf, sbuf2);
        printf(sbuf);

        sprintf(sbuf, " \nDEBUG: %d to %d  \n", mfrom, mto);
        printf(sbuf);
          sprintf(sbuf,"  EPsq: %c%c (%3d)\n",
                        'a'+(Rootep&7), '8'-(Rootep>>4&7), Rootep); printf(sbuf);
          sprintf(sbuf,"  RemP: %c%c (%3d)\n\n",
                        'a'+(RemP&7), '8'-(RemP>>4&7), RemP); printf(sbuf);
      }
      else printf("\n\nILLEGAL!\n");

   }
}



int main(int argc, char **argv)
{
   chess();
   return 0;


}
              
