//////////
//TETRIS//
//////////


//Includes
#include <iostream>
#include <chrono>
#include <thread>
#include <conio.h>
#include <windows.h>
using namespace std;


//classes
class tetronimo {
    public:
    char shape[20];
    char letter;

    void init(string s, char l){

        for (int i = 0; i < 16; i++){

            shape[i] = s[i];
            letter = l;
            
        }


    }

};



class board {

    public:

        //Tetronimo variables
        tetronimo current_piece;
        tetronimo shape_list[1];
        int current_x;
        int current_y;


        //Board variables
        bool first_frame = true;
        int spawn_colum;
        int board_height = 20;
        int board_width = 8;

        //Board arrays
        char cells[20][8];
        char buffer_frame[20][8];
        char landed_blocks[20][8];

        void draw_board() {

            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

            //Loops 160 times, drawing the entire board from a 2D array
            for (int row = 0; row < 20; row++) {

                for (int col = 0; col < 8; col++) {

                    //Checks if game is initializing, or if the current cell is different from the buffer
                    if (first_frame || cells[row][col] != buffer_frame[row][col]) {

                        //Determines the appropriate position for the cursor to overwrite and outputs the changed area
                        COORD coord = { (SHORT)(col * 2), (SHORT)row };
                        SetConsoleCursorPosition(hConsole, coord);
                        cout << cells[row][col];
                        
                        //Updates the buffer frame so it matches ready for the next board draw
                        buffer_frame[row][col] = cells[row][col];

                    }


                }


            }

        }


        //Writes the tetronimo data into cells procedurally
        void draw_tetronimo(bool erasing){

            char fill = erasing ? '.' : current_piece.letter;

            //Loops 16 times, writing only characters matching the current set
            for (int i = 0; i < 16; i++){

                if (current_piece.shape[i] == current_piece.letter){

                    //Values of position in the tetronimo string
                    int shape_x = i % 4;
                    int shape_y = i / 4;

                    //Values of the position on the board relative to tetronimo index
                    int board_x = current_x + shape_x;
                    int board_y = current_y + shape_y;
                    
                    //Checks to see if shape is in bounds
                    if (board_y >= 0 && board_y < 20 && board_x >= 0 && board_x < 8) {
                        cells[board_y][board_x] = fill;

                    }

                }

            }

        }


        //Reinitializes tetronimo values ready to spawn through draw method
        void spawn_tetronimo(){

            current_x = spawn_colum;
            current_y = 0;
            draw_tetronimo(false);

        }

        void lock_block(){

            for(int n = 0; n< 16; n++){

                if(current_piece.shape[n] == current_piece.letter){

                    int colum = current_x + (n % 4);
                    int row = current_y + (n / 4);

                    if (row >= 0 && row < 20 && colum >= 0 && colum < 8){

                        landed_blocks[row][colum] = current_piece.letter;

                    }
                }

            }

        }


        //Checks the area surrounding the tetronimo for colliding objects (the floor, shapes, etc.)
        bool check_collider(){

            int temp_y = current_y + 1;



            cout<<"c";
            //Loops 16 times, writing only characters matching the current set
            for (int n = 0; n < 16; n ++){
                if (current_piece.shape[n] == current_piece.letter){

                    //Values of position in the tetronimo string
                    int shape_x = n % 4;
                    int shape_y = n / 4;

                    //Values of the position on the board relative to tetronimo index
                    int board_x = current_x + shape_x;
                    int board_y = temp_y + shape_y;

                    //Checks the tetronimo is within boundaries of the board
                    if (board_x <= 0 || board_x >= 8 || board_y >= 20){

                        return true;
                        cout<<"d";

                    }

                    //Checks area surrounding the tetronimo
                    if (board_y > 0 && landed_blocks[board_y][board_x] != '.'){

                        cout<<"e";
                        return true;

                    }


                }

            }

            //No collision detected
            cout<<"f";
            return false;

        }


        //Returns a tetronimo from the list based on random chance rules
        tetronimo get_tetronimo(){

            return shape_list[0];

        }


        //Constructor, initializing dependant variables
        void init(){
            
            spawn_colum = (board_width / 2) - 2;

            //Fills cells, buffer, and block array with empty space
            for (int row = 0; row < 20; row++) {

                for (int col = 0; col < 8; col++) {

                    cells[row][col] = '.';
                    buffer_frame[row][col] = '.';
                    landed_blocks[row][col] = '.';

                }

            }

        //Creates shapes in memory
        tetronimo first;
        first.init(".........XX..XX.",'X');
        current_piece = first;

        }



};


//functions
int main() {

    //Game initializing
    board Board;
    Board.init();
    Board.draw_board();
    Board.spawn_tetronimo();
    Board.draw_board();
    
    //Game loop
    while(1){

        //this_thread::sleep_for(std::chrono::milliseconds(250));
        getchar();

        if(!Board.check_collider()){

            Board.draw_tetronimo(true);
            Board.current_y += 1;
            Board.draw_tetronimo(false);

        }

        else{

            Board.lock_block();
            Board.spawn_tetronimo();

        }

        Board.draw_board();

    }

    return 0;

}