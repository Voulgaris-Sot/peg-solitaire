/*TODO 
 *  1)Check malloc for Null
 *  2)Should use a struct to represent a move, not a int
 *  3)More general way to set up board and check if finished
 *  4)Remember the boards I have already see to be more efficient
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define PAWN '+'
#define ILLEGAL '-'
#define EMPTY 'O'

#define x_size 7
#define y_size 7
/*
 x
 |
 |
 V
 -->y
 */

struct list{
    int move;
    struct list *next;
};

typedef struct list list;
/*
struct move{
    int x_start,x_end,y_start,y_end;
}

typedef struct move move;
Be carefull of conflicts with the word move
*/

//Global variables.Might change them.
long int win=0;
list *win_moves=NULL;

//declarations of functions
void decode_move(int move,int *x_start,int *y_start,int *x_end,int *y_end);
list *find_moves(char board[][y_size]);
void print_board(char board[][y_size]);
void make_move(char board[][y_size],int move);
void undo_move(char board[][y_size],int move);
void solver(char board[][y_size],int num_moves);
void fill_board(char board[][y_size]);

/*    PSEUDOCODE
    for each move and while i havent found a win{
        make a move from move list
        solver();
        if(win)
            print move (or put it in an array)
        undo move
    }*/

//basic dfs solver.Might change how it checks for win,in order to make it more portable
void solver(char board[][y_size],int num_moves)
{
    list *available_moves,*current;
    //check for win
    if( num_moves==31 ){
        if(board[3][3]==PAWN){
            win=1;
        }
        return ;
    }

    available_moves=find_moves(board);

    for(current=available_moves;current!=NULL && win!=1;current=current->next){
        make_move(board,current->move);
        solver(board,num_moves+1);
        if(win!=1){
            undo_move(board,current->move);
        }else{//record the move
            list *temp=(list *)malloc(sizeof(list));
            temp->move=current->move;
            temp->next=win_moves;
            win_moves=temp;
            undo_move(board,current->move);
            break;
        }
    }
    //free memory
    while(available_moves != NULL){
        current = available_moves;
        available_moves = available_moves->next;
        free(current);
    }

}

//It takes a move and decodes it in the coordinates of the board
//The moves are in this form ABCD where A is x_start,B is y_start,C is x_end and D is y_end
//I should change it if i want boards with dimensions higher than 10
//Could instead use a struct
void decode_move(int move,int *x_start,int *y_start,int *x_end,int *y_end)
{
    *y_end=move%10;
    move/=10;
    *x_end=move%10;
    move/=10;
    *y_start=move%10;
    move/=10;
    *x_start=move;
}

//It receives a position on the board and then it return a list with all the possible moves
list *find_moves(char board[][y_size])
{
    int i,j;
    list *head,*temp;
    head=NULL;

    for(i=0;i<x_size;i++){
        for(j=0;j<y_size;j++){
            if(board[i][j]==PAWN){
                //check for right captures
                if( (j+2)<y_size &&  board[i][j+1]==PAWN && board[i][j+2]==EMPTY){
                    temp=(list *)malloc(sizeof(list));
                    temp->move=i*1000+j*100+i*10+(j+2);
                    temp->next=head;
                    head=temp;
                }
                //check for left captures
                if( (j-2)>=0 &&  board[i][j-1]==PAWN && board[i][j-2]==EMPTY){
                    temp=(list *)malloc(sizeof(list));
                    temp->move=i*1000+j*100+i*10+(j-2);
                    temp->next=head;
                    head=temp;
                }
                //check for down captures
                if( (i+2)<x_size &&  board[i+1][j]==PAWN && board[i+2][j]==EMPTY){
                    temp=(list *)malloc(sizeof(list));
                    temp->move=i*1000+j*100+(i+2)*10+j;
                    temp->next=head;
                    head=temp;                   
                }
                //check for up captures
                if( (i-2)>=0 &&  board[i-1][j]==PAWN && board[i-2][j]==EMPTY){
                    temp=(list *)malloc(sizeof(list));
                    temp->move=i*1000+j*100+(i-2)*10+j;
                    temp->next=head;
                    head=temp;                  
                }
            }
        }
    }
    return head;
}

int main()
{
    int i,x_start,y_start,x_end,y_end;
    char board[x_size][y_size];
    list *current;

    fill_board(board);
    puts("------------INITIAL BOARD------------");
    print_board(board);

    solver(board,0);

    current=win_moves;
    printf("\n\nThe winning moves are\n\n");
    for( i=1 ; current != NULL; current = current->next,i++) {
        printf("--------%d MOVE---------\n",i);
        make_move(board,current->move);
        print_board(board);
        decode_move(current->move,&x_start,&y_start,&x_end,&y_end);
        printf("From (%d,%d) to (%d,%d)\n",x_start,y_start,x_end,y_end);
        //if(i!=31)
        //    system( "read -n 1 -s   " );//wait for input
    }
    
    //free memory
    while(win_moves != NULL){
        current=win_moves;
        win_moves=win_moves->next;
        free(current);
    }

    return 0;
}

//self explanatory
void make_move(char board[][y_size],int move)
{
    int x_start,y_start,x_end,y_end;
    
    decode_move(move,&x_start,&y_start,&x_end,&y_end);
    
    if(x_start==x_end){
        if( y_end>y_start){ //RIGHT MOVE
            board[x_start][y_start]=EMPTY;
            board[x_start][y_start+1]=EMPTY;
            board[x_end][y_end]=PAWN;
        }else{ //LEFT MOVE
            board[x_start][y_start]=EMPTY;
            board[x_start][y_start-1]=EMPTY;
            board[x_end][y_end]=PAWN;
        }
    }
    else{
        if(x_start > x_end){ //UP MOVE
            board[x_start][y_start]=EMPTY;
            board[x_start-1][y_start]=EMPTY;
            board[x_end][y_end]=PAWN;
        }else{ //DOWN MOVE
            board[x_start][y_start]=EMPTY;
            board[x_start+1][y_start]=EMPTY;
            board[x_end][y_end]=PAWN;
        }
    }
}

//mirror of make move except it un-dones the move.Could merge them in one.
void undo_move(char board[][y_size],int move)
{
    
    int x_start,y_start,x_end,y_end;
    
    decode_move(move,&x_start,&y_start,&x_end,&y_end);
    
    if(x_start==x_end){
        if( y_end>y_start){ //RIGHT MOVE
            board[x_start][y_start]=PAWN;
            board[x_start][y_start+1]=PAWN;
            board[x_end][y_end]=EMPTY;
        }else{ //LEFT MOVE
            board[x_start][y_start]=PAWN;
            board[x_start][y_start-1]=PAWN;
            board[x_end][y_end]=EMPTY;
        }
    }
    else{
        if(x_start > x_end){ //UP MOVE
            board[x_start][y_start]=PAWN;
            board[x_start-1][y_start]=PAWN;
            board[x_end][y_end]=EMPTY;
        }else{ //DOWN MOVE
            board[x_start][y_start]=PAWN;
            board[x_start+1][y_start]=PAWN;
            board[x_end][y_end]=EMPTY;
        }
    }
}

//sets up the initial state of the board
void fill_board(char board[][y_size])
{
    int i,j;
    
    for(i=0;i<x_size;i++){
        for(j=0;j<y_size;j++){
            board[i][j]=PAWN;
        }
    }
    board[0][0]=board[0][1]=board[1][0]=board[1][1]=ILLEGAL;
    board[x_size-1][0]=board[x_size-1][1]=board[x_size-2][0]=board[x_size-2][1]=ILLEGAL;
    board[0][y_size-1]=board[0][y_size-2]=board[1][y_size-1]=board[1][y_size-2]=ILLEGAL;
    board[x_size-1][y_size-1]=board[x_size-1][y_size-2]=board[x_size-2][y_size-1]=board[x_size-2][y_size-2]=ILLEGAL;
    
    board[x_size/2][y_size/2]=EMPTY;//only Empty in the middle
}

//prints the current state of the board
void print_board(char board[][y_size])
{
    int i,j;
    
    printf("  0 1 2 3 4 5 6\n");
    for(i=0;i<x_size;i++){
        for(j=0;j<y_size+1;j++){
            if(j==0)
                printf("%d|",i);
            else
                printf("%c|",board[i][j-1]);
        }
        printf("\n");
    }
}
