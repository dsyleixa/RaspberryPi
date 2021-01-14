/***************************************************************************/
/*                               micro-Max,                                */
/* A chess program orig. <  2 KByte (of non-blank source), by H.G. Muller  */
/***************************************************************************/
/* version 4.8.j2  ported and reworked for RasPi by dsyleixa               */
/* features:                                                               */
/* version 4.8  features:                                                  */
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
#include <readline/readline.h>
#include <readline/history.h>


#define Kb(A,B) *(int*)(T+A+(B&8)+S*(B&7))


#define HASHSIZE (1<<24) //  wegen RAM, für PC: (1<<24) // <<<<<<  für Arduino Mega 1<<8 !

#define WHITE 16
#define BLACK 8

struct HTab {
          int  Key,
               Score;
          int  From,
               To,
               Draft;
        } HashTab[HASHSIZE];                             // hash table, HTsize entries 



//  think time, std for PC: 60000 
#define MAXNODE  1700000L   // max deepening: increased values => higher skills

int  K,
     RootEval,
     R,
     HashKeyLo,
     HashKeyHi;

int32_t   N,
          INF=8000;                                     // INF=8000: "infinity eval score"   ;

int
    bchk=136,                                            // board check: bchk=0x88   board system     
    S=128,                                               // dummy square 0x80, highest valid square =127     
    Side=16;                                             // 16=white, 8=black; Side^=24 switches ther and back

signed char  L;

                         // piece list (number, type) is {1,2,3,4,5,6,7} = {P+,P-,N,K,B,R,Q}. 
                         // Type 0 is not used, so it can indicate empty squares on the board.
signed char  
     pval[]={0,2,2,7,-1,8,12,23},                        // relative piece values     
                                        // step-vect list:
                                        // piece type p finds starting dir[j] at [p+16]
     pvect[]={-16,-15,-17,0,                             // pawns
	          1,16,0,                                    // rook
              1,16,15,17,0,                              // king, queen and bishop
	          14,18,31,33,0,                             // knight   
              7,-1,11,6,8,3,6},                          // 1st dir. in pvect[] per piece 

     stdBaseLn[]={6,3,5,7,4,5,3,6},     // initial piece setup  
          
     board[129],                                         // board: half of 16x8+dummy 
     T[1035];                                            // hash translation table    

signed char  psymbol[]= ".?+nkbrq?*?NKBRQ";              // .empty ?undef +downstreamPawn *upstreamPawn
                                                         // nKnight kKing bBishop rRook qQueen 

int  mfrom, mto;       // current ply from - to
int  Rootep,           // e.PieceType. square
     rmvP=128;         // remove piece




// recursive minimax search, Side=moving side, n=depth 

int  Minimax(int32_t  Alpha, int32_t  Beta, int32_t  Eval, int  epSqr, int  prev, int32_t   Depth)
                       // (Alpha,Beta)=window, Eval, EnPass_sqr. 
                       // prev=prev.dest; HashKeyLo,HashKeyHi=hashkeys; return EvalScore 
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

 struct HTab *a = HashTab + ((HashKeyLo + Side * epSqr) & (HASHSIZE-1)); // lookup pos. in hash table, improved

   //char sbuf[50];



   Alpha--;                                             // adj. window: delay bonus  
   Side^=24;                                            // change sides              
   IterDepth  = a->Draft;
   BestScore  = a->Score;
   BestFrom   = a->From;
   BestTo     = a->To;                                  // resume at stored depth    

   if(a->Key-HashKeyHi | prev |                         // miss: other pos. or empty 
      !(BestScore<=Alpha | BestFrom&8&&BestScore>=Beta | BestFrom&S))            //   or window incompatible  
      { IterDepth=BestTo=0; }                           // start iter. from scratch  

   BestFrom&=~bchk;                                        // start at best-move hint   

   while( IterDepth++ <  Depth || IterDepth<3           // iterative deepening loop  
     || prev&K == INF
       && ( (N<MAXNODE  && IterDepth<98)                   // root: deepen upto time; changed from N<60000    
          || (K=BestFrom, L=BestTo&~bchk, IterDepth=3)
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
            j = pvect[PieceType+16];                    // first step pvect f.piece 
            while(StepVec = PieceType>2 & StepVec<0 ? -StepVec : -pvect[++j] )       // loop over directions pvect[]  
            {
labelA:                                                                 // resume normal after best  
               ToSqr =FromSqr ;                                         // (FromSqr ,ToSqr )=move          
               SkipSqr= RookSqr =S;                                     // (SkipSqr, RookSqr )=castl.R       

               do
               {                                                        // ToSqr  traverses ray, or:      
                  CaptSqr=ToSqr =h?BestTo^h:ToSqr +StepVec;             // sneak in prev. best move  

                  if(ToSqr &bchk)break;                                 // board edge hit            

                  BestScore= epSqr-S&board[epSqr]&&ToSqr -epSqr<2&epSqr-ToSqr <2?INF:BestScore;      // bad castling              
                  
                  if( PieceType<3 && ToSqr==epSqr) CaptSqr^=16;         // CaptSqr for E.P. if Eval of PieceType=Pawn. 
                                                                        // <3 is a piecetype which represent pawns (1,2) 
                                                                        // (officers are >=3: 6,3,5,7,4)
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
                     if(!( RookSqr & bchk))board[SkipSqr]=Side+6,v+=50;               // castling: put R & Eval   
                     v-=PieceType-4|R>29?0:20;                                     // penalize mid-game K move  

                     if(PieceType<3)                                               // pawns:                    
                     {
                        v-=9*((FromSqr -2 & bchk||board[FromSqr -2]-Piece)+           // structure, undefended     
                               (FromSqr +2 & bchk||board[FromSqr +2]-Piece)-1         //        squares plus bias  
                              +(board[FromSqr ^16]==Side+36))                      // kling to non-virgin King  
                              -(R>>2);                                             // end-game Pawn-push bonus  
                         V=ToSqr +StepVec+1 & S?647-PieceType:2*(Piece & ToSqr +16 & 32);           // promotion or 6/7th bonus  
                         board[ToSqr ]+=V;
                         i+=V;                                                     // change piece, add Eval   
                     }

                     v+= Eval+i;
                     V=BestScore >Alpha ? BestScore  : Alpha;                      // new eval and alpha        
                     HashKeyLo+=Kb(ToSqr +0,board[ToSqr ])-Kb(FromSqr +0,Piece)-Kb(CaptSqr +0,Victim );
                     HashKeyHi+=Kb(ToSqr +8,board[ToSqr ])-Kb(FromSqr +8,Piece)-Kb(CaptSqr +8,Victim )+ RookSqr -S;  // update hash key           
                     C = IterDepth-1-(IterDepth>5 & PieceType>2 & !Victim & !h);
                     C = R>29|IterDepth<3|P-INF?C:IterDepth;                       // extend 1 ply if in check  
                     do {
                        s= (C>2)||(v>V)? 
                             -Minimax(-Beta,-V,-v, SkipSqr,0,C)              // recursive eval. of reply                                                                                    
                              :v;                                            // or fail low if futile     
                     } while( s>Alpha && ++C<IterDepth );

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
                     BestScore = v, BestFrom=FromSqr, BestTo=(ToSqr | (S & SkipSqr));   // mark double move with S   
                  }
                  if(h)
                  {
                     h=0;
                     goto labelA;                                       // redo after doing old best 
                  }
                  if (
                    FromSqr + StepVec - ToSqr  || Piece & 32 ||               // not 1st step,moved before 
                    PieceType>2 && (PieceType-4 | j-7 ||                      // no P & no lateral K move, 
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

      } while( (FromSqr = (FromSqr +9) & ~bchk)-StartSqr );                // next sqr. of board, wrap  

labelC:
      if (BestScore >INF-bchk || BestScore <bchk-INF) IterDepth=98;           // mate holds to any depth   
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
         printf("\n%2d ply, searched: %9d ", IterDepth-1, N-S );
       }
       else
       if( ((N-S)%10000)<1) { printf("."); fflush(stdout); }


   }  // while (iterative deepening loop)

   Side^=24;                                                            // change sides back         
   mfrom=K; mto=L;
   return BestScore += BestScore <Eval;                                 // delayed-loss bonus        
}


//------------------------------------------------------------
void standardBoard() {                                // initial board setup      
   int col=8;                                         // count by column (=file)  
   memset(board, 0, sizeof(board));        
   while(col--) {                                    
      board[col]=(board[col+112]=stdBaseLn[col]+8)+8; // 1st+8th line (=rank): pcs by setup series
      board[col+16]=18;                               // 2nd line: black P-
      board[col+96]=9;                                // 7th line: white P+  
   }  
}


//------------------------------------------------------------
int pieceToVal(char p) {
    // {r,n,b,q,k,+,R,N,B,Q,K,*} => { (+16:) 6,3,5,7,4,2,  (+8:) 6,3,5,7,4,1 }
    // psymbol[]= ".?+nkbrq?*?NKBRQ"
    // 16=white, 8=black
    
    // default=0, for all the rest, e.g.:
    // if(p=='.') return 0;
    // if(p==' ') return 0;
    
    if(p=='r') return 6+16;
    if(p=='n') return 3+16;
    if(p=='b') return 5+16;
    if(p=='q') return 7+16;
    if(p=='k') return 4+16;
    if(p=='+') return 18;  // black P-

    if(p=='R') return 6+8;
    if(p=='N') return 3+8;
    if(p=='B') return 5+8;
    if(p=='Q') return 7+8;
    if(p=='K') return 4+8;
    if(p=='*') return 9;   // white P+ 
    
    //default:
    return 0;

}

//------------------------------------------------------------

void inputNewboard() {
    char lines[8][9] = {0};
    
    memset(board, 0, sizeof(board));

    for( int i = 0; i < 8; ++i )
      memset(lines[i], '.', 8);

    puts("\n\n");
    for( int i = 7; i>=0; i-- )
    {
      char prompt[10];
      sprintf(prompt, "%d: ", i+1);
      char * const line = readline(prompt);
      if( line == NULL )
        break;
      const size_t len = strlen(line);
      memcpy(lines[i], line, len > 8 ? 8 : len);
      free(line);
    }
    puts("\n\n");

    puts("New Board:");
    for( int i = 7; i>=0; i-- )
      printf("%d: %s\n", i+1, lines[i]);
    puts("\n\n");
    
    memset(board, 0, sizeof(board));

    for( int c = 0; c < 8; ++c )   board[c]    =pieceToVal(lines[7][c]);    
    for( int c = 0; c < 8; ++c )   board[c+16] =pieceToVal(lines[6][c]);
    for( int c = 0; c < 8; ++c )   board[c+32] =pieceToVal(lines[5][c]);
    for( int c = 0; c < 8; ++c )   board[c+48] =pieceToVal(lines[4][c]);
    for( int c = 0; c < 8; ++c )   board[c+64] =pieceToVal(lines[3][c]);
    for( int c = 0; c < 8; ++c )   board[c+80] =pieceToVal(lines[2][c]);
    for( int c = 0; c < 8; ++c )   board[c+96] =pieceToVal(lines[1][c]);
    for( int c = 0; c < 8; ++c )   board[c+112]=pieceToVal(lines[0][c]);
       
}

    


//------------------------------------------------------------
void centerPointsTable() {                            // center-points table 
   int col=8, x;                                      // (in unused half board[])
   while(col--) {                                                  
	  x=8;
      while(x--) { 
	     board[16*x + col+8]=(col-4)*(col-4)+(x-3.5)*(x-3.5);           
      }  
   } 	  
}

//------------------------------------------------------------
void hashTblInit() {
   int h=1035;
   while(h-- > bchk) T[h]=rand()>>9;	           // board check: bchk=136=0x88 board system  
}




//------------------------------------------------------------
// chess game user interface
//------------------------------------------------------------
void chess()
{
   int32_t       eval;
   //int16_t       oldto, oldEPSQ; //debug
   char          sbuf[50], sbuf2[50];
   int16_t       str[20], *ptr;
   signed char   oboard[129], spiece;

 
RESTART:
   standardBoard();                                               // standard board setup
   
CUSTOM:      
   centerPointsTable();                                           // center-points table   
                                                                  //(in unused half board[]) 
   hashTblInit();                                                 // init hash table 
   
                                                                        // play loop   
   Side=WHITE;                                                                            
   while(1)
   {
     N=-1;

     printf("\n");

     printf("     A B C D E F G H \n     --------------- \n");  
     while(++N<121) {                                            // print board  
         if(N & 8 && (N+7!=0) ) {
             printf("%3d \n", 1+((120-N)>>4)); 
             N+=7; 
         }
         else {
           if(N%8==0) {
               printf("%3d ", 1+((120-N)>>4)); 
           }
           printf(" %c", psymbol[board[N] & 15]);        
         }
     }
     printf("     --------------- \n     A B C D E F G H \n");  
     printf(" R_estart  Q_uit  I_nput  S_ide \n"); 

SIDE:
     if(Side==16) printf(">  WHITE: ");  
     else         printf(">  BLACK: ");     
    
     ptr=str;
     while( ( *ptr++ = getchar() ) >10 ) ;
    
     if( str[0]=='Q') return;              //  Q=Quit
     if( str[0]=='R') goto RESTART;        //  R=Restart
     if( str[0]=='I') { inputNewboard();   //  I=Input
                        goto CUSTOM;    } 
     if( str[0]=='S') { Side^=24;          //  switch side
                        goto SIDE; }      
     if( str[0]==10 ) str[0]=0;            //  Nullstring => auto move generator

     K=INF;
      
     
     if(str[0]!=0) {                                                // parse entered move  
       K= str[0]-16*str[1]+799;
       L= str[2]-16*str[3]+799;     
     }
    
     memcpy(oboard, board, sizeof(board));
     //oldto=mto;
     //oldEPSQ=Rootep;

     eval=Minimax(-INF, INF, RootEval, Rootep, 1, 3);      // think or check & do 

     if(eval!=15) {
        if(oboard[mto])   rmvP=mto;
        else  rmvP=128;
        //debug: //if(mto==oldEPSQ)  rmvP=oldto;
        spiece=psymbol[board[mto] & 15];
        if(spiece=='*' || spiece=='+') spiece=' ';           
        sprintf(sbuf,"\n\nmove: %c %c%c", spiece,'a'+(mfrom & 7),'8'-(mfrom>>4));

        if(oboard[mto]) strcat(sbuf," X ");
        else strcat(sbuf,"-");
        sprintf(sbuf2,"%c%c (%d-%d)", 'a'+(mto & 7),'8'-(mto>>4 & 7), mfrom, mto);
        strcat(sbuf, sbuf2);
        printf("%s", sbuf);

        //printf(" \nDEBUG: %d to %d  \n", mfrom, mto);
          printf("  EPsq: ");
          if(Rootep!=128)
             printf("%c%c (%d)", 'a'+(Rootep & 7), '8'-(Rootep>>4 & 7), Rootep);  
          else printf ("(  )");   
          printf("  rmvP: ");
          if(rmvP!=128) 
             printf("%c%c (%3d)\n\n", 'a'+(rmvP & 7), '8'-(rmvP>>4 & 7), rmvP);  
          else printf ("(  )"); 
          printf("\n");
      }
      else printf("\n\nILLEGAL!\n");

   }
}




int main(int argc, char **argv)
{
   chess();
   return 0;


}


/* comments:
Reading the input move

The input code just reads characters upto a newline, and then combines first with second and third with fourth to get the move K,L. 
The conversion from ASCII to a square number involves a constant that is so close to the value of a Queen minus Pawn (799 in stead of (9-1)*99 = 792) 
that is serves a double use. 
Before parsing the move micro-Max checks, however, if the input consisted of a single newline, in which case it thinks up a move itself (move generator)
by calling D() with the invalid sqauare number 8 in the capture square z. 
This also puts a move in the global variables K,L.
At any move the program is thus ready to accept either an input move in algabraic notation, or to start thinking and playing his own (on an empty input line). 
This makes it easy to set up a starting position, change sides during the game, or have the computer play against itself. 


center-pts table: Positional Points for Pieces in Good Places

To make for at least vaguely sensical positional play in the opening and mid-game, the program needs a clue as to where its pieces are best deployed. 
A simple way that is in common use to do this are 'piece-square' tables, arrays that for each piece type tabulate the desirability to have that piece on a certain square. 
In Micro-Max all piece-square tables are condensed into a single one, used for all pieces except Rook and Queen. 
The latter don't care where the stand, and thus need no table at all. 
This 'çenter-positional-score' table, since it is indexed by square-number, also occupies blocks of 8 values separated by spaces of 8 unused elements 
(for the invalid square-numbers). 
It thus fits nicely in the unused elements of b[], interleaving the game board with the positional table. 
i.e. the positional score for square x can be found in b[x+8], while the piece on it is found in b[x].

The value to be on a particular square decreases faster if we approach the board edge. 
This has the effect that in an end-game against a bare King, the engine finds it important to keep that King against the edge or in the corner, 
even if it has to move his own King slightly off center to achieve this. Unfortunately Kings are also attracted by the center in the middle game... 
*/            
/*   http://home.hccnet.nl/h.g.muller/umax4_8.c                            */
/*   https://home.hccnet.nl/h.g.muller/chess.html                          */
/*   https://home.hccnet.nl/h.g.muller/progress.html                       */
              
