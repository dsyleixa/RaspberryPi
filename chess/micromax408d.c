/***************************************************************************/
/*                               micro-Max,                                */
/* A chess program orig. <  2 KByte (of non-blank source), by H.G. Muller  */
/***************************************************************************/
/* version 4.8.d  RasPi   features:                               */
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

#define HTsize (1<<12) //  wegen RAM, für PC: (1<<24) // <<<<<<  für Arduino Mega verkleinern auf 1<<8 !

struct HTab {
          int  K,
               V;
          int  X,
               Y,
               D;
        } HTarray[HTsize];           /* hash table, HTsize entries*/


#define MAXNODES 80000   //  wegen Zugdauer; für PC: x10 = 800000 = 8e5   // <<<<<<  für Mega verkleinern auf 10000!

int  K,
     Q,
     R,
     J,
     Z;

int32_t    N,
     I=80000;                         /* I=80000: "infinity score" */ ;

int
    M=136,                           /* M=0x88   board system    */
    S=128,                           /* dummy square 0x80, highest valid square =127    */
    turn=16;                         // 16=Weiss, 8=Schwarz; turn^=24 wechselt hin unnd her

signed char  L,
     pval[]={0,2,2,7,-1,8,12,23},                      /* relative piece values    */
     vector[]={-16,-15,-17,0,1,16,0,1,16,15,17,0,14,18,31,33,0, /* step-vector lists */
          7,-1,11,6,8,3,6},                          /* 1st dir. in vector[] per piece*/

     bsetup[]={6,3,5,7,4,5,3,6},                         /* initial piece setup      */
     board[129],                                        /* board: half of 16x8+dummy*/
     T[1035];                                        /* hash translation table   */

signed char  psymbol[]= ".?+nkbrq?*?NKBRQ";

int  mfrom, mto;    // current ply from - to
int  EPSQ,           // e.p. square
     RemP;           // remove piece




/* recursive minimax search, turn=moving side, n=depth*/

int  Minimax(int32_t  q, int32_t  l, int32_t  score, int  EPC, int  prev, int32_t  hashkey)
                       /* (q,l)=window, score, EnPass_sqr.*/
                       /* prev=prev.dest; J,Z=hashkeys; return score*/
{
   int  j,
        r,
        m,
        v,
        d,
        h,
        i,
        F,
        G,
        V,
        P,
        f=J,
        g=Z,
        C,
        s;
   signed char  t,
        p,
        upiece,
        x,
        y,
        X,
        Y,
        H,
        B;

   struct HTab *a = HTarray + (J + turn * EPC & HTsize-1);   /* lookup pos. in hash table*/

   char sbuf[50];



   q--;                                          /* adj. window: delay bonus */
   turn^=24;                                        /* change sides             */
   d=a->D;
   m=a->V;
   X=a->X;
   Y=a->Y;                  /* resume at stored depth   */

   if(a->K-Z|prev|                                  /* miss: other pos. or empty*/
      !(m<=q | X&8&&m>=l | X&S))                   /*   or window incompatible */
      { d=Y=0; }                                /* start iter. from scratch */

   X&=~M;                                        /* start at best-move hint  */

   while( d++ < hashkey || d<3                   /* iterative deepening loop */
     || prev&K == I
       && ( N<60000 & d<98                        /* root: deepen upto time   */
          || (K=X, L=Y&~M, d=3)
       )
     )                                          /* time's up: go do best    */
   {
      x=B=X;                                       /* start scan at prev. best */
      h=Y&S;                                       /* request try noncastl. 1st*/
      P=d<3 ? I : Minimax(-l,1-l,-score,S,0,d-3);               /* Search null move         */
      m = (-P<l | R>35) ? ( d>2 ? -I : score ) : -P;            /* Prune or stand-pat       */
      N++;                                         /* node count (for timing)  */
      do
      {
         upiece=board[x];                                   /* scan board looking for   */
         if(upiece & turn)                                  /*  own piece (inefficient!)*/
         {
            r = p = upiece&7;                               /* p = piece type (set r>0) */
            j = vector[p+16];                                 /* first step vector f.piece*/
            while(r = p>2 & r<0 ? -r : -vector[++j] )       /* loop over directions vector[] */
            {
labelA:                                        /* resume normal after best */
               y=x;                            /* (x,y)=move         */
               F=G=S;                          /* (F,G)=castl.R      */

               do
               {                                       /* y traverses ray, or:     */
                  H=y=h?Y^h:y+r;                           /* sneak in prev. best move */

                  if(y&M)break;                            /* board edge hit           */

                  m= EPC-S&board[EPC]&&y-EPC<2&EPC-y<2?I:m;      /* bad castling             */

                  if(p<3&y==EPC)H^=16;                           /* shift capt.sqr. H if e.p.*/

                  t=board[H];

                  if(t&turn|p<3&!(y-x&7)-!t)break;            /* capt. own, bad pawn mode */
                  i=37*pval[t&7]+(t&192);                     /* value of capt. piece t   */
                  m=i<0?I:m;                                  /* K capture                */

                  if(m>=l&d>1) goto labelC;                     /* abort on fail high       */

                  v=d-1?score:i-p;                             /* MVV/LVA scoring          */

                  if(d-!t>1)                               /* remaining depth          */
                  {
                     v=p<6?board[x+8]-board[y+8]:0;                  /* center positional pts.   */
                     board[G]=board[H]=board[x]=0;board[y]=upiece|32;             /* do move, set non-virgin  */
                     if(!(G&M))board[F]=turn+6,v+=50;               /* castling: put R & score  */
                     v-=p-4|R>29?0:20;                              /* penalize mid-game K move */

                     if(p<3)                                        /* pawns:                   */
                     {
                        v-=9*((x-2&M||board[x-2]-upiece)+              /* structure, undefended    */
                               (x+2&M||board[x+2]-upiece)-1            /*        squares plus bias */
                              +(board[x^16]==turn+36))                 /* kling to non-virgin King */
                              -(R>>2);                                 /* end-game Pawn-push bonus */
                         V=y+r+1&S?647-p:2*(upiece&y+16&32);           /* promotion or 6/7th bonus */
                         board[y]+=V;
                         i+=V;                                         /* change piece, add score  */
                     }

                     v+= score+i;
                     V=m>q ? m : q;                           /* new eval and alpha       */
                     J+=K(y+0,board[y])-K(x+0,upiece)-K(H+0,t);
                     Z+=K(y+8,board[y])-K(x+8,upiece)-K(H+8,t)+G -S;  /* update hash key          */
                     C=d-1-(d>5&p>2&!t&!h);
                     C=R>29|d<3|P-I?C:d;                     /* extend 1 ply if in check */
                     do {
                        s=C>2|v>V?-Minimax(-l,-V,-v,         /* recursive eval. of reply */
                                            F,0,C):v;        /* or fail low if futile    */
                     } while( s>q & ++C<d );

                     v=s;
                     if(prev&&K-I&&v+I&&x==K&y==L)              /* move pending & in root:  */
                     {
                        Q=-score-i; EPSQ=F;                            /*   exit if legal & found  */
                        a->D=99;a->V=0;                        /* lock game in hash as draw*/
                        R+=i>>7;
                        return l;                            /* captured non-P material  */
                     }
                     J=f;
                     Z=g;                                    /* restore hash key         */
                     board[G]=turn+6;
                     board[F]=board[y]=0;
                     board[x]=upiece;
                     board[H]=t;                             /* undo move,G can be dummy */
                  }
                  if(v>m)                                  /* new best, update max,best*/
                  {
                     m=v,X=x,Y=y|S&F;                      /* mark double move with S  */
                  }
                  if(h)
                  {
                     h=0;
                     goto labelA;                           /* redo after doing old best*/
                  }
                  if (
                    x+r-y|upiece&32|                             /* not 1st step,moved before*/
                    p>2 & (
                      p-4|j-7||                             /* no P & no lateral K move,*/
                      board[G=x+3^r>>1&7]-turn-6            /* no virgin R in corner G, */
                      || board[G^1] | board[G^2] )          /* no 2 empty sq. next to R */
                    )
                  {
                     t+=p<5;
                  }                                        /* fake capt. for nonsliding*/
                  else F=y;                                /* enable e.p.              */

               } while(!t);                               /* if not capt. continue ray*/

            }
         }  // (upiece & turn)

      } while((x=x+9&~M)-B);                       /* next sqr. of board, wrap */

labelC:
      if (m>I-M|m<M-I) d=98;                       /* mate holds to any depth  */
      m= m+I|P==I ? m : 0;                         /* best loses K: (stale)mate*/

      if(a->D<99) {                                /* protect game history     */
         a->K=Z;
         a->V=m;
         a->D=d;                       /* always store in hash tab */
         a->X=X|8*(m>q)|S*(m<l);
         a->Y=Y;                       /* move, type (bound/exact),*/
      }
      /* uncomment for Kibitz */
      //if(prev) sprintf(sbuf, "%2d ply, %9d searched, score=%6d by %c%c%c%c\n",
      //     d-1, N-S, m, 'a'+(X&7),'8'-(X>>4),'a'+(Y&7),'8'-(Y>>4&7));

       if(prev  && X!=Y) {
         sprintf(sbuf, "\n%2d ply, searched: %9d ", d-1, N-S );
         printf(sbuf);
       }
       else
       if( ((N-S)%10000)<1) printf(".");


   }  // while (iterative deepening loop)

   turn^=24;                                        /* change sides back        */
   mfrom=K; mto=L;
   return m+= m<score;                                  /* delayed-loss bonus       */
}





void chess()
{
   int32_t       score, i;
   int16_t       oldto, oldEPSQ;
   char          sbuf[50], sbuf2[50];
   int16_t       cstring[20], *p;
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

     sprintf(sbuf,"     A B C D E F G H \n     --------------- \n"); printf(sbuf);
     while(++N<121) {                                            /* print board */
         if(N&8 && (N+7!=0) ) {sprintf(sbuf,"%3d \n", 1+((120-N)>>4)); printf(sbuf); N+=7; }
         else {
           if(N%8==0) {sprintf(sbuf, "%3d ", 1+((120-N)>>4)); printf(sbuf); }
         sprintf(sbuf," %c", psymbol[board[N]&15]);         printf(sbuf);
         }
     }
     sprintf(sbuf,"     --------------- \n     A B C D E F G H "); printf(sbuf);

     if(turn==16) sprintf(sbuf,"\n>  WHITE: ");  else sprintf(sbuf,"\n>   BLACK:  ");
     printf(sbuf);

     i = 0;
     
     p=cstring;
     while( ( *p++ = getchar() ) >10 );
     if(cstring[0]==10)cstring[0]=0;

     K=I;

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
     oldEPSQ=EPSQ;

     score=Minimax(-I, I, Q, EPSQ, 1, 3);                              /* think or check & do*/

     if(score!=15) {
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
                        'a'+(EPSQ&7), '8'-(EPSQ>>4&7), EPSQ); printf(sbuf);
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
              
