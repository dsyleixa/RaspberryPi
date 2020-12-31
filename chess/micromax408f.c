/***************************************************************************/
/*                               micro-Max,                                */
/* A chess program orig. <  2 KByte (of non-blank source), by H.G. Muller  */
/***************************************************************************/
/* version 4.8.f  RasPi   features:                                        */

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
/*   http://home.hccnet.nl/h.g.muller/umax4_8.c  
/*   https://home.hccnet.nl/h.g.muller/chess.html                            */

// Code für Raspberry Pi

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#define K(A,B) *(int*)(T+A+(B&8)+S*(B&7))

#define HASHSIZE (1<<24) //  wegen RAM, für PC: (1<<24) // <<<<<<  für Arduino Mega 1<<8 !

struct HTab {
          int  Key,
               Score;
          int  From,
               To,
               Draft;
        } HashTab[HASHSIZE];                             // hash table, HTsize entries 


#define MAXNODES 800000   //  wegen Zugdauer; für PC: x10 = 800000 = 8e5   // <<<<<<  für Mega verkleinern auf 10000!

int  K,
     RootEval,
     R,
     HashKeyLo,
     HashKeyHi;

int32_t   N,
          INF=80000;                                     // INF=80000: "infinity e"   ;

int
    M=136,                                               // M=0x88   board system     
    S=128,                                               // dummy square 0x80, highest valid square =127     
    Side=16;                                             // 16=Weiss, 8=Schwarz; Side^=24 wechselt hin unnd her

signed char  L,
     pval[]={0,2,2,7,-1,8,12,23},                        // relative piece values     
     vector[]={-16,-15,-17,0,1,16,0,1,16,15,17,0,14,18,31,33,0, // step-vector lists  
          7,-1,11,6,8,3,6},                              // 1st dir. in vector[] per piece 

     bsetup[]={6,3,5,7,4,5,3,6},                         // initial piece setup       
     board[129],                                         // board: half of 16x8+dummy 
     T[1035];                                            // hash translation table    

signed char  psymbol[]= ".?+nkbrq?*?NKBRQ";

int  mfrom, mto;    // current ply from - to
int  Rootep,           // e.PieceType. square
     rmvP;           // remove piece




// recursive minimax search, Side=moving side, n=depth 

int  Minimax(int32_t  Alpha, int32_t  Beta, int32_t  Eval, int  epSqr, int  prev, int32_t   Depth)
                       // (Alpha,Beta)=window, Eval, EnPass_sqr. 
                       // prev=prev.dest; HashKeyLo,HashKeyHi=hashkeys; return e 
{
   int  j,
        StepVec,
        BestScore,
        v,
        IterDepth,
        h,
        i,
        SkipSqr,
        RookSqr,
        V,
        P,            // Null Move Prawning
        f=HashKeyLo,
        g=HashKeyHi,
        C,
        s;
   signed char  Victim,
        PieceType,
        Piece,
        FromSqr ,
        ToSqr ,
        BestFrom,
        BestTo,
        CaptSqr,
        StartSqr ;

   struct HTab *a = HashTab + (HashKeyLo + Side * epSqr & HASHSIZE-1);   // lookup pos. in hash table 

   char sbuf[50];



   Alpha--;                                             // adj. window: delay bonus  
   Side^=24;                                            // change sides              
   IterDepth  = a->Draft;
   BestScore  = a->Score;
   BestFrom   = a->From;
   BestTo     = a->To;                                  // resume at stored depth    

   if(a->Key-HashKeyHi|prev|                            // miss: other pos. or empty 
      !(BestScore<=Alpha | BestFrom&8&&BestScore>=Beta | BestFrom&S))                   //   or window incompatible  
      { IterDepth=BestTo=0; }                           // start iter. from scratch  

   BestFrom&=~M;                                        // start at best-move hint   

   while( IterDepth++ <  Depth || IterDepth<3           // iterative deepening loop  
     || prev&K == INF
       && ( N<60000 & IterDepth<98                      // root: deepen upto time    
          || (K=BestFrom, L=BestTo&~M, IterDepth=3)
       )
     )                                                  // time's up: go do best     
   {
      FromSqr =StartSqr =BestFrom;                      // start scan at prev. best  
      h=BestTo&S;                                       // request try noncastl. 1st 
      P=IterDepth<3 ? INF : Minimax(-Beta,1-Beta,-Eval,S,0,IterDepth-3);            // Search null move          
      BestScore = (-P<Beta | R>35) ? ( IterDepth>2 ? -INF : Eval ) : -P;            // Prune or stand-pat        
      N++;                                              // node count (for timing)   
      do
      {
         Piece=board[FromSqr ];                         // scan board looking for    
         if(Piece & Side)                               //  own piece (inefficient!) 
         {
            StepVec = PieceType = Piece&7;              // PieceType = piece type (set StepVec>0)  
            j = vector[PieceType+16];                   // first step vector f.piece 
            while(StepVec = PieceType>2 & StepVec<0 ? -StepVec : -vector[++j] )       // loop over directions vector[]  
            {
labelA:                                                                 // resume normal after best  
               ToSqr =FromSqr ;                                         // (FromSqr ,ToSqr )=move          
               SkipSqr= RookSqr =S;                                     // (SkipSqr, RookSqr )=castl.R       

               do
               {                                                        // ToSqr  traverses ray, or:      
                  CaptSqr=ToSqr =h?BestTo^h:ToSqr +StepVec;             // sneak in prev. best move  

                  if(ToSqr &M)break;                                    // board edge hit            

                  BestScore= epSqr-S&board[epSqr]&&ToSqr -epSqr<2&epSqr-ToSqr <2?INF:BestScore;      // bad castling              

                  if(PieceType<3&ToSqr ==epSqr)CaptSqr^=16;             // shift capt.sqr. CaptSqr if Eval.PieceType. 

                  Victim =board[CaptSqr];

                  if(Victim & Side|PieceType<3 & !(ToSqr -FromSqr & 7)-!Victim )break;            // capt. own, bad pawn mode  
                  i=37*pval[Victim & 7]+(Victim & 192);                 // value of capt. piece Victim     
                  BestScore =i<0?INF:BestScore ;                        // K capture                 

                  if(BestScore >=Beta & IterDepth>1) goto labelC;       // abort on fail high        

                  v=IterDepth-1?Eval:i-PieceType;                       // MVV/LVA scoring           

                  if(IterDepth-!Victim >1)                              // remaining depth           
                  {   
                     v=PieceType<6?board[FromSqr +8]-board[ToSqr +8]:0;            // center positional pts.    
                     board[ RookSqr ]=board[CaptSqr ]=board[FromSqr ]=0;board[ToSqr ]=Piece|32;             // do move, set non-virgin   
                     if(!( RookSqr & M))board[SkipSqr]=Side+6,v+=50;               // castling: put R & Eval   
                     v-=PieceType-4|R>29?0:20;                                     // penalize mid-game K move  

                     if(PieceType<3)                                               // pawns:                    
                     {
                        v-=9*((FromSqr -2 & M||board[FromSqr -2]-Piece)+           // structure, undefended     
                               (FromSqr +2 & M||board[FromSqr +2]-Piece)-1         //        squares plus bias  
                              +(board[FromSqr ^16]==Side+36))                      // kling to non-virgin King  
                              -(R>>2);                                             // end-game Pawn-push bonus  
                         V=ToSqr +StepVec+1 & S?647-PieceType:2*(Piece & ToSqr +16 & 32);           // promotion or 6/7th bonus  
                         board[ToSqr ]+=V;
                         i+=V;                                                     // change piece, add Eval   
                     }

                     v+= Eval+i;
                     V=BestScore >Alpha ? BestScore  : Alpha;                      // new eval and alpha        
                     HashKeyLo+=K(ToSqr +0,board[ToSqr ])-K(FromSqr +0,Piece)-K(CaptSqr +0,Victim );
                     HashKeyHi+=K(ToSqr +8,board[ToSqr ])-K(FromSqr +8,Piece)-K(CaptSqr +8,Victim )+ RookSqr -S;  // update hash key           
                     C = IterDepth-1-(IterDepth>5 & PieceType>2 & !Victim & !h);
                     C = R>29|IterDepth<3|P-INF?C:IterDepth;                       // extend 1 ply if in check  
                     do {
                        s=C>2|v>V?-Minimax(-Beta,-V,-v,                            // recursive eval. of reply  
                                            SkipSqr,0,C):v;                        // or fail low if futile     
                     } while( s>Alpha & ++C<IterDepth );

                     v=s;
                     if(prev && K-INF && v+INF && (FromSqr==K) & (ToSqr==L) )      // move pending & in root:   
                     {
                        RootEval=-Eval-i; Rootep=SkipSqr;               // exit if legal & found   
                        a->Draft=99;a->Score=0;                         // lock game in hash as draw 
                        R+=i>>7;
                        return Beta;                                    // captured non-P material   
                     }
                     HashKeyLo=f;
                     HashKeyHi=g;                                       // restore hash key          
                     board[ RookSqr ]=Side+6;
                     board[SkipSqr]=board[ToSqr ]=0;
                     board[FromSqr ]=Piece;
                     board[CaptSqr ]=Victim ;                           // undo move, RookSqr can be dummy  
                  }
                  if(v>BestScore )                                      // new best, update max,best 
                  {
                     BestScore =v,BestFrom=FromSqr ,BestTo=ToSqr |S & SkipSqr;                      // mark double move with S   
                  }
                  if(h)
                  {
                     h=0;
                     goto labelA;                                       // redo after doing old best 
                  }
                  if (
                    FromSqr + StepVec - ToSqr  || Piece & 32 ||               // not 1st step,moved before 
                    PieceType>2 &  (PieceType-4 | j-7 ||                      // no P & no lateral K move, 
                      board[ RookSqr =FromSqr +3^StepVec>>1 & 7]-Side-6       // no virgin R in corner  RookSqr,  
                      || board[ RookSqr^1] || board[ RookSqr^2] )             // no 2 empty sq. next to R  
                    )
                  {
                     Victim += PieceType<5;
                  }                                                     // fake capt. for nonsliding 
                  else SkipSqr=ToSqr ;                                  // enable e.PieceType.               

               } while(!Victim );                                       // if not capt. continue ray 

            }
         }  // (Piece &  Side)

      } while( (FromSqr =FromSqr +9 & ~M)-StartSqr );                   // next sqr. of board, wrap  

labelC:
      if (BestScore >INF-M || BestScore <M-INF) IterDepth=98;           // mate holds to any depth   
      BestScore = BestScore +INF || P==INF ? BestScore  : 0;            // best loses K: (stale)mate 

      if(a->Draft<99) {                                                 // protect game history      
         a->Key=HashKeyHi;
         a->Score=BestScore ;
         a->Draft=IterDepth;                                            // always store in hash tab  
         a->From=BestFrom|8*(BestScore >Alpha)|S*(BestScore <Beta);
         a->To=BestTo;                                                  // move, type (bound/exact), 
      }
      // uncomment for Kibitz  
      // if(prev) sprintf(sbuf, "%2d ply, %9d searched, score=%6d by %c%c%c%c\n",
      //     IterDepth-1, N-S, BestScore , 'a'+(BestFrom & 7),'8'-(BestFrom>>4),'a'+(BestTo & 7),'8'-(BestTo>>4 & 7));

       if(prev  && BestFrom!=BestTo) {
         sprintf(sbuf, "\n%2d ply, searched: %9d ", IterDepth-1, N-S );
         printf(sbuf);
       }
       else
       if( ((N-S)%10000)<1) { printf("."); fflush(stdout); }


   }  // while (iterative deepening loop)

   Side^=24;                                                            // change sides back         
   mfrom=K; mto=L;
   return BestScore += BestScore <Eval;                                 // delayed-loss bonus        
}





void chess()
{
   int32_t       eval;
   int16_t       oldto, oldEPSQ;
   char          sbuf[50], sbuf2[50];
   int16_t       cstring[20], *ptr;
   signed char   oboard[129], spiece;


   K=8;
   while(K--)
   {
      board[K]=(board[K+112]=bsetup[K]+8)+8;
      board[K+16]=18;
      board[K+96]=9;                                                    // initial board setup 
      L=8;
      while(L--)board[16*L+K+8]=(K-4)*(K-4)+(L-3.5)*(L-3.5);            // center-pts table    
   }                                                                    //(in unused half board[]) 
   N=1035;
   while(N-->M)T[N]=rand()>>9;

                                                                        // play loop           
   while(1)
   {
     N=-1;

     printf("\n");

     sprintf(sbuf,"     A B C D E F G H \n     --------------- \n"); printf(sbuf);
     while(++N<121) {                                            // print board  
         if(N & 8 && (N+7!=0) ) {sprintf(sbuf,"%3d \n", 1+((120-N)>>4)); printf(sbuf); N+=7; }
         else {
           if(N%8==0) {sprintf(sbuf, "%3d ", 1+((120-N)>>4)); printf(sbuf); }
         sprintf(sbuf," %c", psymbol[board[N] & 15]);         printf(sbuf);
         }
     }
     sprintf(sbuf,"     --------------- \n     A B C D E F G H "); printf(sbuf);

     if(Side==16) sprintf(sbuf,"\n>  WHITE: ");  else sprintf(sbuf,"\n>   BLACK:  ");
     printf(sbuf);

    
     ptr=cstring;
     while( ( *ptr++ = getchar() ) >10 ) ;
    
     if( cstring[0]=='Q') return;
     if( cstring[0]==10 ) cstring[0]=0; 


     K=INF;
      
     
     if(cstring[0]!=0) {                                                // parse entered move  
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

     eval=Minimax(-INF, INF, RootEval, Rootep, 1, 3);      // think or check & do 

     if(eval!=15) {
        rmvP=S;
        if(oboard[mto])   rmvP=mto;
        if(mto==oldEPSQ)  rmvP=oldto;

        spiece=psymbol[board[mto] & 15];
        if(spiece=='*' || spiece=='+') spiece=' ';
        sprintf(sbuf,"\n\nmoved: >> %c %c%c", spiece,'a'+(mfrom & 7),'8'-(mfrom>>4) );

        if(oboard[mto]) strcat(sbuf," X ");
        else strcat(sbuf,"-");

        sprintf(sbuf2,"%c%c ", 'a'+(mto & 7),'8'-(mto>>4 & 7));
        strcat(sbuf, sbuf2);
        printf(sbuf);

        sprintf(sbuf, " \nDEBUG: %d to %d  \n", mfrom, mto);
        printf(sbuf);
          sprintf(sbuf,"  EPsq: %c%c (%3d)\n",
                        'a'+(Rootep & 7), '8'-(Rootep>>4 & 7), Rootep); printf(sbuf);
          sprintf(sbuf,"  rmvP: %c%c (%3d)\n\n",
                        'a'+(rmvP & 7), '8'-(rmvP>>4 & 7), rmvP); printf(sbuf);
      }
      else printf("\n\nILLEGAL!\n");

   }
}



int main(int argc, char **argv)
{
   chess();
   return 0;


}
              
