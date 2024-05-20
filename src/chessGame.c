#include "chessGame.h"

void initialize_game(ChessGame *game) {
    game->chessboard [0][0] = 'r';
    game->chessboard [0][1] = 'n';
    game->chessboard [0][2] = 'b';
    game->chessboard [0][3] = 'q';
    game->chessboard [0][4] = 'k';
    game->chessboard [0][5] = 'b';
    game->chessboard [0][6] = 'n';
    game->chessboard [0][7] = 'r';
    for(int i = 0; i < 8; i++){
        game->chessboard [1][i] = 'p';
    }
    for(int i = 2; i < 6; i++){
        for(int j = 0; j < 8; j++){
            game->chessboard[i][j] = '.';
        }
    }
    for(int i = 0; i < 8; i++){
        game->chessboard [6][i] = 'P';
    }
    game->chessboard [7][0] = 'R';
    game->chessboard [7][1] = 'N';
    game->chessboard [7][2] = 'B';
    game->chessboard [7][3] = 'Q';
    game->chessboard [7][4] = 'K';
    game->chessboard [7][5] = 'B';
    game->chessboard [7][6] = 'N';
    game->chessboard [7][7] = 'R';

    // Set other game state variables
    game->moveCount = 0;
    game->capturedCount = 0;
    game->currentPlayer = WHITE_PLAYER;
}

void chessboard_to_fen(char fen[], ChessGame *game) {
    int index = 0;
    for(int i = 0; i < 8; i++){
        int space_count = 0;
        for(int j = 0; j < 8; j++){
            if(game->chessboard[i][j] != '.'){
                if(space_count > 0){
                    fen[index] = '0' + space_count;
                    index++;
                    space_count = 0;
                }
                fen[index] = game->chessboard [i][j];
                index++;
            }else{
                space_count++;
            }
        }
        if(space_count > 0){
            fen[index] = '0' + space_count;
            index++;
        }
        if(i < 7){
            fen[index] = '/';
            index++;
        }
    }
    fen[index] = ' ';
    index++;
    if(game->currentPlayer == WHITE_PLAYER){
        fen[index] = 'w';
    }else{
        fen[index] = 'b';
    }
    index++;
    fen[index] = '\0';
}

bool is_valid_pawn_move(char piece, int src_row, int src_col, int dest_row, int dest_col, ChessGame *game) {
    // If in same column and check if the destination square is empty
    if ( dest_col == src_col && game->chessboard[dest_row][dest_col] != '.') {
        return false;
    }
    // If moving diagonal, it has to be non-empty space
    if(dest_col != src_col && game->chessboard[dest_row][dest_col] == '.'){
        return false;
    }

    // White pawn moves
    if (piece == 'P') {
        // Check if moving forward by one square
        if (dest_col == src_col && dest_row == src_row - 1) {
            return true;
        }
        // Check if moving forward by two squares from starting position
        if (src_row == 6 && dest_col == src_col && dest_row == src_row - 2) {
            // Ensure there are no pieces blocking the path
            if (game->chessboard[src_row - 1][src_col] == '.') {
                return true;
            }
        }
        // Check if capturing diagonally
        if (dest_col == src_col + 1 || dest_col == src_col - 1) {
            if (dest_row == src_row - 1 && game->chessboard[dest_row][dest_col] != '.') {
                return true;
            }
        }
    }
    // Black pawn moves
    else if (piece == 'p') {
        // Check if moving forward by one square
        if (dest_col == src_col && dest_row == src_row + 1) {
            return true;
        }
        // Check if moving forward by two squares from starting position
        if (src_row == 1 && dest_col == src_col && dest_row == src_row + 2) {
            // Ensure there are no pieces blocking the path
            if (game->chessboard[src_row + 1][src_col] == '.') {
                return true;
            }
        }
        // Check if capturing diagonally
        if (dest_col == src_col + 1 || dest_col == src_col - 1) {
            if (dest_row == src_row + 1 && game->chessboard[dest_row][dest_col] != '.') {
                return true;
            }
        }
    }

    return false;
}

bool is_valid_rook_move(int src_row, int src_col, int dest_row, int dest_col, ChessGame *game) {
    // Check if moving horizontally or vertically
    if (dest_row == src_row || dest_col == src_col) {
        // Check if there are no pieces blocking the path
        int step = dest_row == src_row ? (dest_col > src_col ? 1 : -1) : (dest_row > src_row ? 1 : -1);
        int r = src_row, c = src_col;
        while (r != dest_row || c != dest_col) {
            r += dest_row == src_row ? 0 : step;
            c += dest_col == src_col ? 0 : step;
            if (game->chessboard[r][c] != '.' && (r != dest_row || c != dest_col)) {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool is_valid_knight_move(int src_row, int src_col, int dest_row, int dest_col) {
     // Check if moving in an L-shape
    int row_diff = dest_row - src_row;
    int col_diff = dest_col - src_col;
    if ((row_diff == 1 && col_diff == 2) || (row_diff == 2 && col_diff == 1) ||
        (row_diff == -1 && col_diff == 2) || (row_diff == -2 && col_diff == 1) ||
        (row_diff == 1 && col_diff == -2) || (row_diff == 2 && col_diff == -1) ||
        (row_diff == -1 && col_diff == -2) || (row_diff == -2 && col_diff == -1)) {
        return true;
    }
    return false;
}

bool is_valid_bishop_move(int src_row, int src_col, int dest_row, int dest_col, ChessGame *game) {
    // Check if moving diagonally
    if (abs(dest_row - src_row) == abs(dest_col - src_col)) {
        // Check if there are no pieces blocking the path
        int step_row = dest_row > src_row ? 1 : -1;
        int step_col = dest_col > src_col ? 1 : -1;
        int r = src_row + step_row, c = src_col + step_col;
        while (r != dest_row || c != dest_col) {
            if (game->chessboard[r][c] != '.' && (r != dest_row || c != dest_col)) {
                return false;
            }
            r += step_row;
            c += step_col;
        }
        return true;
    }
    return false;
}

bool is_valid_queen_move(int src_row, int src_col, int dest_row, int dest_col, ChessGame *game) {
    // A queen can move like a rook or a bishop
    return is_valid_rook_move(src_row, src_col, dest_row, dest_col, game) ||
           is_valid_bishop_move(src_row, src_col, dest_row, dest_col, game);
}

bool is_valid_king_move(int src_row, int src_col, int dest_row, int dest_col) {
    // Check if moving one square in any direction
    int row_diff = abs(dest_row - src_row);
    int col_diff = abs(dest_col - src_col);
    return (row_diff <= 1 && col_diff <= 1);
}

bool is_valid_move(char piece, int src_row, int src_col, int dest_row, int dest_col, ChessGame *game) {
    if(piece == 'P' || piece == 'p'){
        return is_valid_pawn_move(piece,src_row, src_col, dest_row, dest_col, game);
    }
    if(piece == 'B' || piece == 'b'){
        return is_valid_bishop_move(src_row, src_col, dest_row, dest_col, game);
    }
    if(piece == 'Q' || piece == 'q'){
        return is_valid_queen_move(src_row, src_col, dest_row, dest_col, game);
    }
    if(piece == 'K' || piece == 'k'){
        return is_valid_king_move(src_row, src_col, dest_row, dest_col);
    }
    if(piece == 'N' || piece == 'n'){
        return is_valid_knight_move(src_row, src_col, dest_row, dest_col);
    }
    if(piece == 'R' || piece == 'r'){
        return is_valid_rook_move(src_row, src_col, dest_row, dest_col, game);
    }
    return false;
}

void fen_to_chessboard(const char *fen, ChessGame *game) {
    int row = 0;
    int col = 0;
    int index = 0;
    while(row < 8){
        if(fen[index] < '0' || fen[index] > '9'){
            game->chessboard[row][col] = fen[index];
            col++;
            index++;
        }else{
            int num = fen[index] - '0';
            for(int i = 0; i < num; i++){
                game->chessboard[row][col] = '.';
                col++;
            }
            index++;
        }
        if(col > 7){
            row++;
            col = 0;
            index++;
        }
    }
    char player = fen[index];
    if (player == 'w'){
        game->currentPlayer = WHITE_PLAYER;
    }else{
        game->currentPlayer = BLACK_PLAYER;
    }
    
}

int parse_move(const char *move, ChessMove *parsed_move) {
    int length = 0;
    while(move[length] != 0){
        length++;
    }
    if(length != 4 && length != 5){
        return PARSE_MOVE_INVALID_FORMAT;
    }
    if(move[0] < 'a' || move[0] > 'h' || move[2] < 'a' || move[2] > 'h'){
        return PARSE_MOVE_INVALID_FORMAT;
    }
    if(move[1] < '1' || move[1] > '8' || move[3] < '1' || move[3] > '8' ){
        return PARSE_MOVE_OUT_OF_BOUNDS;
    }
    if(length == 5 && move[3] != '1' && move[3] != '8'){
        return PARSE_MOVE_INVALID_DESTINATION;
    }
    if(length == 5 && move[4] != 'q' && move[4] != 'r' && move[4] != 'b' && move[4] != 'n'){
        return PARSE_MOVE_INVALID_PROMOTION;
    }

    parsed_move->startSquare[0] = move[0];
    parsed_move->startSquare[1] = move[1];
    parsed_move->startSquare[2] = 0;
    parsed_move->endSquare[0] = move[2];
    parsed_move->endSquare[1] = move[3];
    
    if(length == 5){
        parsed_move->endSquare[2] = move[4]; 
        parsed_move->endSquare[3] = 0; 
    }else{
        parsed_move->endSquare[2] = 0;
    }
    
    return 0;
}

int make_move(ChessGame *game, ChessMove *move, bool is_client, bool validate_move) {

    // any required variable declarations go here
    int start_col = move->startSquare[0] - 'a';
    int start_row = 7 - (move->startSquare[1] - '1');
    int end_col = move->endSquare[0] - 'a';
    int end_row = 7 - (move->endSquare[1] - '1');
    char piece = game->chessboard[start_row][start_col];
    printf("STAR ROW: %d\n", start_row);
    printf("STAR COL: %d\n", start_col);
    printf("PIECE: %c\n", piece);
    char other_piece =game-> chessboard[end_row][end_col]; //where we are trying to move
    int length = 0;
    while(move->endSquare[length] != 0){
        length++;
    }

    if (validate_move) {
        if(game->currentPlayer == WHITE_PLAYER && is_client == false){
            return MOVE_OUT_OF_TURN;
        }
        if(game->currentPlayer == BLACK_PLAYER && is_client == true){
            return MOVE_OUT_OF_TURN;
        }
        if(game->chessboard[start_row][start_col] == '.'){
            return MOVE_NOTHING;
        }
        if(game->currentPlayer == WHITE_PLAYER && (piece < 'A' || piece > 'Z')){
            return MOVE_WRONG_COLOR;
        }
        if(game->currentPlayer == BLACK_PLAYER && (piece < 'a' || piece > 'z')){
            return MOVE_WRONG_COLOR;
        }
        if(game->currentPlayer == WHITE_PLAYER && other_piece >= 'A' && other_piece <= 'Z'){
            return MOVE_SUS;
        }
        if(game->currentPlayer == BLACK_PLAYER && other_piece >= 'a' && other_piece <= 'z'){
            return MOVE_SUS;
        }
        if(length == 3 && piece != 'P' && piece != 'p'){
            return MOVE_NOT_A_PAWN;
        }
        if(length == 2 &&(end_row == 0 || end_row == 7) && (piece == 'P' || piece == 'p')){
            return MOVE_MISSING_PROMOTION;
        }
        if(!is_valid_move(piece, start_row, start_col, end_row, end_col, game)){
            return MOVE_WRONG;
        }

    }
    if(other_piece != '.'){
        game->capturedPieces[game->capturedCount] = other_piece;
        game->capturedCount++;
    }
    game->chessboard[end_row][end_col] = piece;
    game->chessboard[start_row][start_col] = '.';
    game->moves[game->moveCount] = *move; //moves into the array moves
    game->moveCount++;

    if(length == 3){
        if(game->currentPlayer == WHITE_PLAYER){
            game->chessboard[end_row][end_col] = move->endSquare[2] - 32;
        }else{
            game->chessboard[end_row][end_col] = move->endSquare[2];
        }
    }

    if(game->currentPlayer == WHITE_PLAYER){
        game->currentPlayer = BLACK_PLAYER;
    }else{
        game->currentPlayer = WHITE_PLAYER;
    }

    return 0;
}

int send_command(ChessGame *game, const char *message, int socketfd, bool is_client) {
    char commands[1000];
    int index = 0;
    while(message[index] != ' ' && message[index] != 0){
        commands[index] = message[index];
        index++;
    }
    commands[index] = 0;
    printf("%s\n",commands);
    if(strncmp(commands, "/move", 5) == 0){
        char move[100];
        index++;
        int m = 0;
        while(message[index] != 0){
            move[m] = message[index];
            index++;
            m++;
        }
        move[m] = 0;
        printf("%s\n", move);
        ChessMove cm;
        int valid1 = parse_move(move, &cm);
        printf("%d\n", valid1);
        if(valid1 == 0){
            int valid2 = make_move(game, &cm, is_client, true);
            printf("%d\n", valid2);
            if(valid2 == 0){
                send(socketfd, message, strlen(message), 0);
                return COMMAND_MOVE;
            }
        }

    }else if(strncmp(commands, "/forfeit", 8) == 0){
        send(socketfd, message, strlen(message), 0);
        return COMMAND_FORFEIT;

    } else if(strncmp(commands, "/chessboard", 11) == 0){
        display_chessboard(game);
        return COMMAND_DISPLAY;

    }else if(strncmp(commands, "/import", 7) == 0){
        char fen[100];
        int f = 0;
        index++;
        while(message[index] != 0){
            fen[f] = message[index];
            f++;
            index++;
        }
        fen[f] = 0;

        if(is_client == false){

            fen_to_chessboard(fen,game);
            send(socketfd, message, strlen(message), 0);
            return COMMAND_IMPORT;
        }
    }else if(strncmp(commands, "/load", 5) == 0){
        char username[1000];
        index++;
        int u = 0;
        while(message[index] != ' '){
            if(message[index] == 0){
                return COMMAND_ERROR;
            }
            username[u] = message[index];
            u++;
            index++; 
        }
        username[u] = 0;
        char num[100];
        int n = 0;
        index++;
        while(message[index] != 0){
            num[n] = message[index];
            n++;
            index++;
        }
        num[n] = 0;
        int number = atoi(num);
        int valid = load_game(game, username, "game_database.txt", number);
        if(valid == 0){
            send(socketfd, message, strlen(message), 0);
            return COMMAND_LOAD;
        }

    }else if(strncmp(commands, "/save", 5) == 0){
        char username[100];
        int u = 0;
        index++;
        while(message[index] != 0){
            username[u] = message[index];
            u++;
            index++;
        }
        username[u] = 0;
        int valid = save_game(game, username, "game_database.txt");
        if(valid == 0){
            return COMMAND_SAVE;
        }
    }else{
        return COMMAND_UNKNOWN;
    }
    return COMMAND_ERROR;
}

int receive_command(ChessGame *game, const char *message, int socketfd, bool is_client) {
     char commands[1000];
    int index = 0;
    while(message[index] != ' ' && message[index] != 0){
        commands[index] = message[index];
        index++;
    }
    commands[index] = 0;
    printf("%s\n", commands);
    if(strncmp(commands, "/move", 5) == 0){
        char move[100];
        index++;
        int m = 0;
        while(message[index] != 0){
            move[m] = message[index];
            index++;
            m++;
        }
        move[m] = 0;
        ChessMove cm;
        int valid1 = parse_move(move, &cm);
        printf("%d\n", valid1);
        if(valid1 == 0){
            int valid2 = make_move(game, &cm, !is_client, false);
            printf("%d\n", valid2);
            if(valid2 == 0){
                send(socketfd, message, strlen(message), 0);
                return COMMAND_MOVE;
            }
        }

    }else if(strncmp(commands, "/forfeit", 8) == 0){
        close(socketfd);
        return COMMAND_FORFEIT;

    } else if(strncmp(commands, "/chessboard", 11) == 0){
        display_chessboard(game);
        return COMMAND_DISPLAY;

    }else if(strncmp(commands, "/import", 7) == 0){
        char fen[100];
        int f = 0;
        index++;
        while(message[index] != 0){
            fen[f] = message[index];
            f++;
            index++;
        }
        fen[f] = 0;

        if(is_client != false){

            fen_to_chessboard(fen,game);
            send(socketfd, message, strlen(message), 0);
            return COMMAND_IMPORT;
        }
        //return COMMAND_IMPORT;
    }else if(strncmp(commands, "/load", 5) == 0){
        char username[1000];
        index++;
        int u = 0;
        while(message[index] != ' '){
            if(message[index] == 0){
                return COMMAND_ERROR;
            }
            username[u] = message[index];
            u++;
            index++; 
        }
        username[u] = 0;
        char num[100];
        int n = 0;
        index++;
        while(message[index] != 0){
            num[n] = message[index];
            n++;
            index++;
        }
        num[n] = 0;
        int number = atoi(num);
        int valid = load_game(game, username, "game_database.txt", number);
        if(valid == 0){
            send(socketfd, message, strlen(message), 0);
            return COMMAND_LOAD;
        }

    }else if(strncmp(commands, "/save", 5) == 0){
        char username[100];
        int u = 0;
        index++;
        while(message[index] != 0){
            username[u] = message[index];
            u++;
            index++;
        }
        username[u] = 0;
        int valid = save_game(game, username, "game_database.txt");
        if(valid == 0){
            return COMMAND_SAVE;
        }
    }else{
        return COMMAND_ERROR;
    }
    return COMMAND_ERROR;
}

int save_game(ChessGame *game, const char *username, const char *db_filename) {
    // Open the database file in append mode
    FILE *file = fopen(db_filename, "a");
    if (file == NULL) {
        // Unable to open file
        return -1;
    }

    char fen[100];
    if(strlen(username) == 0){
        return -1;
    }
    int index = 0;
    while(username[index] != 0){
        if(username[index] == ' '){
            return -1;
        }
        index++;
    }
    chessboard_to_fen(fen,game);

    fprintf(file, "%s:%s\n", username, fen);
    
    // Close the file
    fclose(file);

    return 0;
}

int load_game(ChessGame *game, const char *username, const char *db_filename, int save_number) {
   // Open the database file for reading
    FILE *file = fopen(db_filename, "r");
    if (file == NULL) {
        // Unable to open file
        return -1;
    }

    char line[1000];
    int count = 0;
    char fen[100];

    while(fgets(line, sizeof(line), file)){
        if(strncmp(username, line, strlen(username)) == 0){
            count++;
            if(count == save_number){
                int index = strlen(username) + 1;
                int index2 = 0;
                while(line[index] != 0){
                fen[index2] = line[index];
                index++;
                index2++;
                }
                fen_to_chessboard(fen, game);
                return 0;
            }
        }
    }

    fclose(file);
    
    return -1;
}

void display_chessboard(ChessGame *game) {
    printf("\nChessboard:\n");
    printf("  a b c d e f g h\n");
    for (int i = 0; i < 8; i++) {
        printf("%d ", 8 - i);
        for (int j = 0; j < 8; j++) {
            printf("%c ", game->chessboard[i][j]);
        }
        printf("%d\n", 8 - i);
    }
    printf("  a b c d e f g h\n");
}