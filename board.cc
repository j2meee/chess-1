#include "board.h"

int SQ64(const int sq120)
{
    int ar[120] = {
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99,  0,  1,  2,  3,  4,  5,  6,  7, 99,
        99,  8,  9, 10, 11, 12, 13, 14, 15, 99,
        99, 16, 17, 18, 19, 20, 21, 22, 23, 99,
        99, 24, 25, 26, 27, 28, 29, 30, 31, 99,
        99, 32, 33, 34, 35, 36, 37, 38, 39, 99,
        99, 40, 41, 42, 43, 44, 45, 46, 47, 99,
        99, 48, 49, 50, 51, 52, 53, 54, 55, 99,
        99, 56, 57, 58, 59, 60, 61, 62, 63, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99
    };
    return ar[sq120];
}

int SQ120(const int sq64)
{
    int ar[64] = {
        21, 22, 23, 24, 25, 26, 27, 28,
        31, 32, 33, 34, 35, 36, 37, 38,
        41, 42, 43, 44, 45, 46, 47, 48,
        51, 52, 53, 54, 55, 56, 57, 58,
        61, 62, 63, 64, 65, 66, 67, 68,
        71, 72, 73, 74, 75, 76, 77, 78,
        81, 82, 83, 84, 85, 86, 87, 88,
        91, 92, 93, 94, 95, 96, 97, 98
    };
    return ar[sq64];
}

void Board::generateHash()
{
    uint64_t key = 0;
    int piece;
    for(int square = 0; square < 120; square++) {
        piece = board[square];
        if(piece != NO_SQ && piece != EMPTY) {
            key ^= PIECE_KEYS[piece][square];
        }
    }
    if(enPas != NO_SQ) {
        key ^= PIECE_KEYS[EMPTY][enPas];
    }
    if(side == WHITE) {
        key ^= SIDE_KEY;
    }
    hashKey = key;
}

void Board::reset()
{
    for(int i = 0; i < 120; i++) {
        board[i] = NO_SQ;
    }
    for(int i = 0; i < 64; i++) {
        board[SQ120(i)] = EMPTY;
    }
    for(int i = 0; i < 3; i++) {
        pawns[i] = 0ULL;
    }
    for(int i = 0; i < 13; i++) {
        pNum[i] = 0;
        for(int j = 0; j < 10; j++) {
            pList[i][j] = 0;
        }
    }
    side = BOTH;
    ply = 0;
    fiftyMove = 0;
    enPas = NO_SQ;
    castlePerm = 0;
    historyPly = 0;
    hashKey = 0ULL;
}

bool Board::parseFen(const std::string fen)
{
    reset();
    int piece = 0;
    int sq64 = 0;
    int sq120 = 0;
    int rank = RANK_8;
    int file = FILE_A;
    unsigned int index = 0;
    while(index < fen.size()) {
        for(index = 0; rank >= RANK_1; index++) {
            int count = 1;
            switch(fen[index]) {
                case 'p': piece = BP; break;
                case 'r': piece = BR; break;
                case 'n': piece = BN; break;
                case 'b': piece = BB; break;
                case 'q': piece = BQ; break;
                case 'k': piece = BK; break;
                case 'P': piece = WP; break;
                case 'R': piece = WR; break;
                case 'N': piece = WN; break;
                case 'B': piece = WB; break;
                case 'Q': piece = WQ; break;
                case 'K': piece = WK; break;
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                    piece = EMPTY;
                    count = fen[index] - '0';
                    break;
                case '/':
                case ' ':
                    rank--;
                    file = FILE_A;
                    count = 0;
                    break;
                default:
                    std::cout << "FEN Error " << std::endl;
                    return false;
            }
            for(int i = 0; i < count; i++) {
                sq64 = rank * 8 + file;
                sq120 = SQ120(sq64);
                if(piece != EMPTY) {
                    board[sq120] = piece;
                }
                file++;
            }
        }
        side = fen[index] == 'w' ? WHITE: BLACK;
        index += 2;
        for(int i = 0; i < 4; i++) {
            if(fen[index] == ' ') {
                break;
            }
            switch(fen[index]) {
                case 'K': castlePerm |= WKCA; break;
                case 'Q': castlePerm |= WQCA; break;
                case 'k': castlePerm |= BKCA; break;
                case 'q': castlePerm |= BQCA; break;
            }
            index++;
        }
        index++;
        if(fen[index] != '-') {
            file = fen[index] - 'a';
            rank = fen[index + 1] - '1';
            enPas = FR2SQ(file, rank);
            index ++;
        }
        break;
    }
    updateList();
    generateHash();
    return true;
}

void Board::print() const
{
    int square;
    int piece;
    std::cout << std::endl << "     " << (side == WHITE ? "white": "black") << std::endl;
    for(int rank = RANK_8; rank >= RANK_1; rank--) {
        for(int file = FILE_A; file <= FILE_H; file++) {
            square = FR2SQ(file, rank);
            piece = board[square];
            std::cout << PIECE_CHARS[piece] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void Board::updateList()
{
    int piece;
    for(int i = 0; i < 120; i++) {
        piece = board[i];
        if(piece != NO_SQ && piece != EMPTY) {
            pList[piece][pNum[piece]] = i;
            pNum[piece]++;
            if(piece == WP) {
                setBit(pawns[WHITE], SQ64(i));
                setBit(pawns[BOTH], SQ64(i));
            }
            else if(piece == BP) {
                setBit(pawns[BLACK], SQ64(i));
                setBit(pawns[BOTH], SQ64(i));
            }
        }
    }
}

bool Board::sqAttacked(const int square, const int side) const
{
    int targetSquare;
    int piece;
    if(side == WHITE && (board[square - 11] == WP || board[square - 9] == WP)) {
        return true;
    }
    else if(side == BLACK && (board[square + 11] == BP || board[square + 9] == BP)) {
        return true;
    }
    for(int i = 0; i < 8; i++) {
        piece = board[square + MOVE_DIR[WN][i]];
        if(side == WHITE && piece == WN) {
            return true;
        }
        else if(side == BLACK && piece == BN) {
            return true;
        }
    }
    for(int i = 0; i < 4; i++) {
        targetSquare = square + MOVE_DIR[WR][i];
        piece = board[targetSquare];
        while(piece != NO_SQ) {
            if(piece != EMPTY) {
                if(side == WHITE && (board[targetSquare] == WR || board[targetSquare] == WQ)) {
                    return true;
                }
                else if(side == BLACK && (board[targetSquare] == BR || board[targetSquare] == BQ)) {
                    return true;
                }
                break;
            }
            targetSquare += MOVE_DIR[WR][i];
            piece = board[targetSquare];
        }
    }
    for(int i = 0; i < 4; i++) {
        targetSquare = square + MOVE_DIR[WB][i];
        piece = board[targetSquare];
        while(piece != NO_SQ) {
            if(piece != EMPTY) {
                if(side == WHITE && (board[targetSquare] == WB || board[targetSquare] == WQ)) {
                    return true;
                }
                else if(side == BLACK && (board[targetSquare] == BB || board[targetSquare] == BQ)) {
                    return true;
                }
                break;
            }
            targetSquare += MOVE_DIR[WB][i];
            piece = board[targetSquare];
        }
    }
    for(int i = 0; i < 8; i++) {
        piece = board[square + MOVE_DIR[WK][i]];
        if(side == WHITE && piece == WK) {
            return true;
        }
        else if(side == BLACK && piece == BK) {
            return true;
        }
    }
    return false;
}

void Board::hashPiece(const int piece, const int square)
{
    hashKey ^= PIECE_KEYS[piece][square];
}

void Board::hashCastle()
{
    hashKey ^= CASTLE_KEYS[castlePerm];
}

void Board::hashEnPas()
{
    hashKey ^= PIECE_KEYS[EMPTY][enPas];
}

void Board::hashSide()
{
    hashKey ^= SIDE_KEY;
}

void Board::updateCastlePerm(const int to, const int from)
{
    castlePerm &= CASTLE_PERM[to];
    castlePerm &= CASTLE_PERM[from];
}

void Board::addHistory(Undo& undo)
{
    historyPly++;
    history[historyPly] = undo;
}

