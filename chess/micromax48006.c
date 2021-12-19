// https://home.hccnet.nl/h.g.muller/max-src2.html
// https://home.hccnet.nl/h.g.muller/umax4_8.c

/***************************************************************************/
/*                               micro-Max,                                */
/* A chess program smaller than 2KB (of non-blank source), by H.G. Muller  */
/***************************************************************************/
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


// ver 48005
// history
// 48005: basic Negamax from 48001
// 48004: UI string
// 48003: Hash Table: all int, no char
// 48002: print scores 
// 48001: no //#define W, no //#define J(A); chess() in main() 
// 48000: original Muller code, applied to C99 (int32_t, signed char,...)
// 48_c:  original Muller code




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>


//#define W while
#define K(A,B) *(int*)(T+A+(B&8)+S*(B&7))
//#define J(A) K(y+A,board[y])-K(x+A,u)-K(H+A,t)

#define U (1<<10)
struct HT {
           int K,V;        // Key, Score
           int X,Y,D;      // From, To, Draft
         } A[U];           // hash table, 16M+8 entries 

int         M=136,   // M=0x88                
            S=128,
            I=8e3,
            Q,       // RootEval
            O,       // EPSQ
            K,
            N,
            R,
            J,
            Z,
            turn=16;
             

signed char L,
            w[]={0,2,2,7,-1,8,12,23},                      // relative piece values     
            o[]={-16,-15,-17,0,1,16,0,1,16,15,17,0,14,18,31,33,0, // step-vector lists  
                  7,-1,11,6,8,3,6},                        // 1st dir. in o[] per piece 
            bsetup[]={6,3,5,7,4,5,3,6},                    // initial piece setup       
            board[129],                                    // board: half of 16x8+dummy 
            T[1035];                                       // hash translation table    

signed char  psymbol[]= ".?+nkbrq?*?NKBRQ";

int  mfrom, mto;    // buffer for current ply from - to
int  RemP;          // buffer fo remove piece sqr
char sbuf[100];

int Minimax(int32_t q, int32_t l, int32_t e, int E, int z, int32_t n)     // recursive minimax search, turn=moving side, n=depth 
//int q,l,e,E,z,n;        // (q,l)=window, e=current eval. score, E=e.p. sqr. 

{                       // e=score, z=prev.dest; J,Z=hashkeys; return score 
 int j,
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
 signed char t,
             p,
             u,
             x,
             y,
             X,
             Y,
             H,
             B;
 struct HT*a=A+(J+turn*E&U-1);                  // lookup pos. in hash table 

 q--;                                           // adj. window: delay bonus  
 turn^=24;                                         // change sides              
 d=a->D;m=a->V;X=a->X;Y=a->Y;                   // resume at stored depth    
 if(a->K-Z|z|                                   // miss: other pos. or empty 
  !(m<=q|X&8&&m>=l|X&S))                        //   or window incompatible  
  d=Y=0;                                        // start iter. from scratch  
 X&=~M;                                         // start at best-move hint   

 while(d++<n||d<3||                                 // iterative deepening loop  
   z&K==I&&(N<1e6&d<98||                        // root: deepen upto time    
   (K=X,L=Y&~M,d=3)))                           // time's up: go do best     
 {x=B=X;                                        // start scan at prev. best  
  h=Y&S;                                        // request try noncastl. 1st 
  P=d<3?I:Minimax(-l,1-l,-e,S,0,d-3);                 // Search null move          
  m=-P<l|R>35?d>2?-I:e:-P;                      // Prune or stand-pat        
  N++;                                          // node count (for timing)   
  do{u=board[x];                                    // scan board looking for    
   if(u&turn)                                      //  own piece (inefficient!) 
   {r=p=u&7;                                    // p = piece type (set r>0)  
    j=o[p+16];                                  // first step vector f.piece 
    while(r=p>2&r<0?-r:-o[++j])                     // loop over directions o[]  
    {
A:                                              // resume normal after best  
     y=x;F=G=S;                                 // (x,y)=move, (F,G)=castl.R 
     do{                                        // y traverses ray, or:      
      H=y=h?Y^h:y+r;                            // sneak in prev. best move  
      if(y&M)break;                             // board edge hit            
      m=E-S&board[E]&&y-E<2&E-y<2?I:m;              // bad castling              
      if(p<3&y==E)H^=16;                        // shift capt.sqr. H if e.p. 
      t=board[H];if(t&turn|p<3&!(y-x&7)-!t)break;      // capt. own, bad pawn mode  
      i=37*w[t&7]+(t&192);                      // value of capt. piece t    
      m=i<0?I:m;                                // K capture                 
      if(m>=l&d>1)goto C;                       // abort on fail high        

      v=d-1?e:i-p;                              // MVV/LVA scoring           
      if(d-!t>1)                                // remaining depth           
      {v=p<6?board[x+8]-board[y+8]:0;                   // center positional pts.    
       board[G]=board[H]=board[x]=0;board[y]=u|32;              // do move, set non-virgin   
       if(!(G&M))board[F]=turn+6,v+=50;                // castling: put R & score   
       v-=p-4|R>29?0:20;                        // penalize mid-game K move  
       if(p<3)                                  // pawns:                    
       {v-=9*((x-2&M||board[x-2]-u)+                // structure, undefended     
              (x+2&M||board[x+2]-u)-1               //        squares plus bias  
             +(board[x^16]==turn+36))                  // kling to non-virgin King  
             -(R>>2);                           // end-game Pawn-push bonus  
        V=y+r+1&S?647-p:2*(u&y+16&32);          // promotion or 6/7th bonus  
        board[y]+=V;i+=V;                           // change piece, add score   
       }
       v+=e+i;V=m>q?m:q;                        // new eval and alpha        
       J+=K(y+0,board[y])-K(x+0,u)-K(H+0,t);
       Z+=K(y+8,board[y])-K(x+8,u)-K(H+8,t)+G-S;    // update hash key           
       C=d-1-(d>5&p>2&!t&!h);
       C=R>29|d<3|P-I?C:d;                      // extend 1 ply if in check  
       do
        s=C>2|v>V?-Minimax(-l,-V,-v,                  // recursive eval. of reply  
                              F,0,C):v;         // or fail low if futile     
       while(s>q&++C<d);v=s;
       if(z&&K-I&&v+I&&x==K&y==L)               // move pending & in root:   
       {Q=-e-i;O=F;                             //   exit if legal & found   
        a->D=99;a->V=0;                         // lock game in hash as draw 
        R+=i>>7;return l;                       // captured non-P material   
       }
       J=f;Z=g;                                 // restore hash key          
       board[G]=turn+6;board[F]=board[y]=0;board[x]=u;board[H]=t;      // undo move,G can be dummy  
      }
      if(v>m)                                   // new best, update max,best 
       m=v,X=x,Y=y|S&F;                         // mark double move with S   
      if(h){h=0;goto A;}                        // redo after doing old best 
      if(x+r-y|u&32|                            // not 1st step,moved before 
         p>2&(p-4|j-7||                         // no P & no lateral K move, 
         board[G=x+3^r>>1&7]-turn-6                    // no virgin R in corner G,  
         ||board[G^1]|board[G^2])                       // no 2 empty sq. next to R  
        )t+=p<5;                                // fake capt. for nonsliding 
      else F=y;                                 // enable e.p.               
     }while(!t);                                    // if not capt. continue ray 
    }
   }
  }while((x=x+9&~M)-B);                         // next sqr. of board, wrap  
  
C:if(m>I-M|m<M-I)d=98;                          // mate holds to any depth   
  m=m+I|P==I?m:0;                               // best loses K: (stale)mate 
  if(a->D<99)                                   // protect game history      
   a->K=Z,a->V=m,a->D=d,                        // always store in hash tab  
   a->X=X|8*(m>q)|S*(m<l),a->Y=Y;               // move, type (bound/exact), 
   
                                                // uncomment for Kibitz  
                                                // uncomment for Kibitz  
   //                                                
   //if(z) {
   //   printf("%2d ply, %9d searched, score=%6d by %c%c%c%c\n",d-1,N-S,m,
   //  'a'+(X&7),'8'-(X>>4),'a'+(Y&7),'8'-(Y>>4&7));
   //}
   //
   
    if( z  && X!=Y ) {
         //fprintf(stderr, "\n%2d ply, searched: %9d ", d-1, N-S );
          fprintf(stderr, "\n%2d ply, %9d searched, score=%6d by %c%c%c%c\n",d-1,N-S,m,
                 'a'+(X&7),'8'-(X>>4),'a'+(Y&7),'8'-(Y>>4&7));
    }
    //else 
    if( (N%10000)<1) { fprintf(stderr, "."); }            
                                     
 }                                             //    encoded in X S,8 bits 
 turn^=24;                                     // change sides back        
 mfrom=K; mto=L;                               // move sqr buffers
 return m+=m<e;                                // delayed-loss bonus       
}


int chess()
{
   int32_t       score=1, i;
   int16_t       oldto, oldEPSQ;
   int16_t       cstring[20], *p;
   signed char   oboard[129], spiece;
   float         movecnt=1.0;

RESTART:
   K=8;
   movecnt=1.0;
   turn=16;
   memset(board, 0, sizeof(board) );
   
   while(K--)
   {
      board[K]=(board[K+112]=bsetup[K]+8)+8;
      board[K+16]=18;
      board[K+96]=9;                               // initial board setup 
      L=8;
      while(L--)board[16*L+K+8]=(K-4)*(K-4)+(L-3.5)*(L-3.5);     // center-pts table    
   }                                                             //(in unused half board[]) 
   N=1035;                                         
   while(N-->M)T[N]=rand()>>9;                     // Hashtable ranom init

                                                   // play loop           
   while(1)
   {
     N=-1;

     printf("\n");

     sprintf(sbuf,"     A B C D E F G H \n     --------------- \n"); printf(sbuf);
     while(++N<121) {                                            // print board  
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

     if(cstring[0]!=0) {                                   // parse entered move  
       K= cstring[0]-16*cstring[1]+799;
       L= cstring[2]-16*cstring[3]+799;
     }
   
     printf("\n DEBUG cstring : %s", cstring);
     
     if(cstring[0]=='R') {
         printf("\nChess game resset/restarted\n");
         goto RESTART;
     }
     else
     if(cstring[0]=='Q') {
         printf("\nChess game reset/restarted\n");
         goto QUIT;
     }
     sprintf(sbuf,"\n DEBUG K: %d  \n DEBUG L: %d \n",  K, L);
     printf(sbuf);
   
     memcpy(oboard, board, sizeof(board));
     oldto=mto;
     oldEPSQ=O;

     score=Minimax(-I, I, Q, O, 1, 3);          // think or check & do 
     printf("\nscore=%d\n", score);

     // DEBUG
     if(score==15)   {
         sprintf(sbuf,"Illegal(?) score(%d) ", score);
         printf(sbuf);
     }
     if(score==-I)   {
         sprintf(sbuf,"-I !! score(%d) ", score);
         printf(sbuf);
     }
     if(score==I/2)   {
         sprintf(sbuf,"I/2 !! score(%d) ", score);
         printf(sbuf);
     }
     if(score==0  )  {
         sprintf(sbuf,"0 !! score(%d) ", score);
         printf(sbuf);
     }
     
     if(score!=15 && score!=-I && score!=0) {
        RemP=S;
        if(oboard[mto])   RemP=mto;
        if(mto==oldEPSQ)  RemP=oldto;

        spiece=psymbol[board[mto]&15];
        if(spiece=='*' || spiece=='+') spiece=' ';
        sprintf(sbuf,"\n\n %4.1f: %c %c%c", movecnt, spiece,'a'+(mfrom&7),'8'-(mfrom>>4) );
        movecnt+=0.5;

        if(oboard[mto]) strcat(sbuf," X ");
        else strcat(sbuf,"-");
        printf(sbuf);
        sprintf(sbuf,"%c%c ", 'a'+(mto&7),'8'-(mto>>4&7));
        printf(sbuf);

        sprintf(sbuf, " \nDEBUG: %d to %d  \n", mfrom, mto);
        printf(sbuf);
        sprintf(sbuf,"  EPsq: %c%c (%3d)\n",
                        'a'+(O&7), '8'-(O>>4&7), O); printf(sbuf);
        sprintf(sbuf,"  RemP: %c%c (%3d)\n\n",
                        'a'+(RemP&7), '8'-(RemP>>4&7), RemP); printf(sbuf);
      }

   }
QUIT:
   printf("\nChess program terminated\n");
   return 0;
}



int main()
{
   chess();
   return 0;

}
     
