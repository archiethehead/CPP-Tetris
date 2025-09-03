//////////////////
//###-TETRIS-###//
//////////////////


//Includes
#include <iostream>
#include <filesystem>
#include <cstdlib>
#include <ctime>
#include <numeric>
#include <chrono>
#include <thread>
#include <windows.h>
using namespace std;

///////////
//Classes//
///////////


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
        tetronimo shape_list[7];
        int shape_list_selected[7];
        int current_x;
        int current_y;


        //Board variables
        bool first_frame = true;
        int spawn_colum;
        int board_height = 20;
        int board_width = 8;
        int score = 0;

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

            cout<<endl<<endl<<"Score: "<<score;

        }

        //Input logic
        void input_handler(){
            bool moved = false;

            if (GetAsyncKeyState(VK_LEFT) & 0x8000) {

                if (!check_left_collider()) {

                draw_tetronimo(true);
                current_x -= 1;
                draw_tetronimo(false);
                moved = true;

                }

            }

            if (GetAsyncKeyState(VK_RIGHT) & 0x8000){

                if (!check_right_collider()) {

                    draw_tetronimo(true);
                    current_x += 1;
                    draw_tetronimo(false);
                    moved = true;

                }

            }

            if (GetAsyncKeyState(VK_DOWN) & 0x8000){

                if (!check_down_collider()){

                    draw_tetronimo(true);
                    current_y += 1;
                    draw_tetronimo(false);
                    moved = true;

                }

            }

            if (GetAsyncKeyState(VK_UP) & 0x8000){

                draw_tetronimo(true);
                rotate_piece();
                draw_tetronimo(false);
                moved = true;

            }

            if (moved){

                draw_board();

            }


        }

        void rotate_piece(){
            char temp_rotated[16];

            //Rotate the current piece 90 degrees, and save it as temp
            for (int y = 0; y < 4; y++) {

                for (int x = 0; x < 4; x++) {

                    temp_rotated[x + y*4] = current_piece.shape[(3 - y) + x*4];

                }
            }

            //Check collisions with landed_blocks, and decide if rotation is possible
            bool colliding = false;
            for (int n = 0; n < 16; n++){

                if (temp_rotated[n] == current_piece.letter){

                    int board_x = current_x + (n % 4);
                    int board_y = current_y + (n / 4);

                    //Check the piece is within valid board boundaries
                    if (board_x < 0 || board_x >= board_width || board_y < 0 || board_y >= board_height) {

                        colliding = true;

                        break;
                    }

                    //Check the placed_blocks array relatice positions to see if they're empty
                    if (landed_blocks[board_y][board_x] != '.') {

                        colliding = true;

                        break;
                    
                    }

                }

            }

            if (!colliding){

                for (int n = 0; n < 16; n++){
                    
                    current_piece.shape[n] = temp_rotated[n];


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
            current_piece = get_tetronimo();
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

            sync_cells_landed();

        }


        //Checks the area surrounding the tetronimo for colliding objects (the floor, shapes, etc.)
        bool check_down_collider(){

            //Loops 16 times, writing only characters matching the current set
            for (int n = 0; n < 16; n ++){

                if (current_piece.shape[n] == current_piece.letter){

                    //Values of the position on the board relative to tetronimo index
                    int board_x = current_x + (n % 4);
                    int board_y = (current_y + 1) + (n / 4);

                    //Checks the tetronimo is within boundaries of the board
                    if (board_y >= board_height){

                        return true;

                    }

                    //Checks area surrounding the tetronimo
                    if (landed_blocks[board_y][board_x] != '.'){

                        return true;

                    }

                }

            }

            //No collision detected
            return false;

        }

        //Checks the area surrounding the tetronimo for colliding objects (the floor, shapes, etc.)
        bool check_right_collider(){

            //Loops 16 times, writing only characters matching the current set
            for (int n = 0; n < 16; n ++){

                if (current_piece.shape[n] == current_piece.letter){

                    //Values of the position on the board relative to tetronimo index
                    int board_x = (current_x + 1) + (n % 4);
                    int board_y = (current_y) + (n / 4);

                    //Checks the tetronimo is within boundaries of the board
                    if (board_x >= board_width){

                        return true;

                    }

                    //Checks area surrounding the tetronimo
                    if (landed_blocks[board_y][board_x] != '.'){

                        return true;

                    }

                }

            }

            //No collision detected
            return false;

        }


        //Checks the area surrounding the tetronimo for colliding objects (the floor, shapes, etc.)
        bool check_left_collider(){

            //Loops 16 times, writing only characters matching the current set
            for (int n = 0; n < 16; n ++){

                if (current_piece.shape[n] == current_piece.letter){

                    //Values of the position on the board relative to tetronimo index
                    int board_x = (current_x - 1) + (n % 4);
                    int board_y = (current_y) + (n / 4);

                    //Checks the tetronimo is within boundaries of the board
                    if (board_x < 0){

                        return true;

                    }

                    //Checks area surrounding the tetronimo
                    if (landed_blocks[board_y][board_x] != '.'){

                        return true;

                    }

                }

            }

            //No collision detected
            return false;

        }


        //Returns a tetronimo from the list based on random chance rules
        tetronimo get_tetronimo(){
            bool selected = false;
            tetronimo selected_shape;

            if (std::accumulate(shape_list_selected, shape_list_selected + 7, 0) == 0){
                
                for (int n = 0; n < 7; n++){

                    shape_list_selected[n] = 1;

                }


            }

            do{

                int random_number = rand() % 7;

                if (shape_list_selected[random_number] == 1){

                    shape_list_selected[random_number] = 0;
                    selected_shape = shape_list[random_number];
                    break;

                }


            }while (!selected);

            return selected_shape;

        }

        bool check_game_over(){

            for (int n = 0; n < 16; n++){

                if(current_piece.shape[n] == current_piece.letter){

                    int board_x = spawn_colum + (n % 4);
                    int board_y = (n / 4);

                    //Check if the cells about the be spawned in are occupied
                    if (landed_blocks[board_y][board_x] != '.') {

                        return true;

                    }

                }

            }

            return false;

        }

        //Checks for complete rows and ammends score
        void check_row_complete(){
            bool row_cleared = false;
            do {

                row_cleared = false;
                
                //Procedurally scan each line of the board, from bottom to top
                for(int row = (board_height - 1); row >= 0; row--){

                    //Check to see if the current row is full
                    bool row_complete = true;

                    for (int colum = 0; colum < board_width; colum++){

                        if (landed_blocks[row][colum] == '.'){

                            row_complete = false;
                            break;

                        }

                    }

                    if (row_complete){

                        //Clears the row detected as full
                        for (int colum = 0; colum < board_width; colum++){

                            landed_blocks[row][colum] = '.';

                        }

                        //Applies gravity to all the rows above
                        for (int r = row; r > 0; r--) {

                            for (int colum = 0; colum < board_width; colum++) {

                                landed_blocks[r][colum] = landed_blocks[r - 1][colum];

                            }
                        }

                        // Clears the top row out of the check range
                        for (int col = 0; col < board_width; col++) {

                            landed_blocks[0][col] = '.';

                        }
                        
                        row_cleared = true;
                        score += 100;
                        break;

                    }

                }

            } while (row_cleared);

            sync_cells_landed();

        }

        void sync_cells_landed() {

            for (int row = 0; row < board_height; row++) {

                for (int colum = 0; colum < board_width; colum++) {

                    cells[row][colum] = landed_blocks[row][colum];

                }

            }

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

            for (int n = 0; n < 7; n++){

                shape_list_selected[n] = 1;

            }

        //Creates shapes in memory
        tetronimo first;
        tetronimo second;
        tetronimo third;
        tetronimo fourth;
        tetronimo fifth;
        tetronimo sixth;
        tetronimo seventh;


        first.init(".XX..XX.........",'X');
        second.init("O...O...O...O...",'O');
        third.init("....L...L...LL..",'L');
        fourth.init(".......R...R..RR",'R');
        fifth.init(".........ZZ.ZZ..",'Z');
        sixth.init("........QQ...QQ.",'Q');
        seventh.init(".........D..DDD.",'D');
        



        shape_list[0] = first;
        shape_list[1] = second;
        shape_list[2] = third;
        shape_list[3] = fourth;
        shape_list[4] = fifth;
        shape_list[5] = sixth;
        shape_list[6] = seventh;

        }



};

/////////////
//Functions//
/////////////

//Tick-timer, courtesy of the internet
bool time_for_tick(int interval) {

    using game_clock = std::chrono::steady_clock;

    static auto last_tick = game_clock::now();
    auto now = game_clock::now();

    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - last_tick).count() >= interval) {

        last_tick = now;
        return true;

    }

    return false;

}


//Code from the C++ website forums, hides the console cursor
void Show_console_cursor(bool showFlag)
{
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_CURSOR_INFO     cursorInfo;

    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = showFlag; // set the cursor visibility
    SetConsoleCursorInfo(out, &cursorInfo);
}


//Main functions
int main() {

    //Game initializing
    srand(time(nullptr));
    Show_console_cursor(false); 

    filesystem::path executable_path = filesystem::current_path() / "Tetris.wav";

    if (filesystem::exists(executable_path)) {

        PlaySound(TEXT("Tetris.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
    
    }

    bool game_over = false;
    board Board;
    Board.init();

    Board.draw_board();
    Board.spawn_tetronimo();
    Board.draw_board();
    
    //Game loop
    while(!game_over){

        this_thread::sleep_for(std::chrono::milliseconds(100));

        Board.input_handler();

        if (time_for_tick(100)){
            if(!Board.check_down_collider()){

                Board.draw_tetronimo(true);
                Board.current_y += 1;
                Board.draw_tetronimo(false);

            }

            else{

                Board.lock_block();
                Board.check_row_complete();
                Board.spawn_tetronimo();

            }
        
        }

        Board.draw_board();
        
        game_over = Board.check_game_over();


    }

    cout<<endl<<endl<<"GAME OVER . . .";
    getchar();

    return 0;

}