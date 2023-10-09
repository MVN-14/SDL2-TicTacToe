#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>
#include <vector>

namespace {
    struct Piece {
        std::string type{""};
        int x{0}, y{0}, cell{0};
    };

    int boardX{300};
    int boardY{200};
    int boardWidth{400};
    int boardHeight{400};
    int cellSize = boardWidth / 3;
    int lineThickness{4};

    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *pieceFont;
    TTF_Font *titleFont;

    bool xTurn{true};
    bool hasWinner{false};

    std::vector<Piece> pieces{{}, {}, {}, {}, {}, {}, {}, {}, {}};

    void DrawTicTacToeBoard() {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

        // Vertical Lines
        SDL_Rect rect{boardX + cellSize - lineThickness / 2,
                      boardY,
                      lineThickness,
                      boardHeight};
        SDL_RenderFillRect(renderer, &rect);
        rect.x += cellSize + lineThickness;
        SDL_RenderFillRect(renderer, &rect);

        // Horiziontal Lines
        rect.x = boardX;
        rect.y = boardY + (boardHeight / 3) - lineThickness / 2;
        rect.h = lineThickness;
        rect.w = boardWidth;
        SDL_RenderFillRect(renderer, &rect);
        rect.y += (boardHeight / 3) + lineThickness;
        SDL_RenderFillRect(renderer, &rect);

        for (auto const &piece : pieces) {
            if (piece.type == "") {
                continue;
            }
            SDL_Surface *text = TTF_RenderText_Solid(pieceFont,
                                                     piece.type.c_str(),
                                                     SDL_Color{255, 255, 255});
            if (!text) {
                std::cerr << "Failed to render text" << TTF_GetError() << '\n';
                return;
            }

            SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, text);
            SDL_Rect dest{piece.x, piece.y, text->w, text->h};

            SDL_RenderCopy(renderer, textTexture, NULL, &dest);
            SDL_DestroyTexture(textTexture);
            SDL_FreeSurface(text);
        }

        SDL_SetRenderDrawColor(renderer, 25, 25, 25, SDL_ALPHA_OPAQUE);
    }

    void reset() {
        pieces = {{}, {}, {}, {}, {}, {}, {}, {}, {}};
        xTurn = true;
        hasWinner = false;
    }

    void DrawTitle() {
        std::string headingText;

        if (hasWinner) {
            headingText = xTurn ? "O Wins" : "X Wins";
        }
        else {
            headingText = xTurn ? "X's Turn" : "O's Turn";
        }

        SDL_Surface *text = TTF_RenderText_Solid(titleFont,
                                                 headingText.c_str(),
                                                 SDL_Color{255, 255, 255});
        if (!text) {
            std::cerr << "Failed to render text" << TTF_GetError() << '\n';
            return;
        }
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, text);

        SDL_Rect dest{boardX, 50, text->w, text->h};

        SDL_RenderCopy(renderer, textTexture, NULL, &dest);

        SDL_DestroyTexture(textTexture);
        SDL_FreeSurface(text);
    }

    int GetCell(int x, int y) {
        if (x < boardX || x > boardX + boardWidth || y < boardY ||
            y > boardY + boardWidth) {
            return -1;
        }

        // first row
        if (y < boardY + (cellSize)) {
            if (x < boardX + cellSize) {
                return 1;
            }
            if (x < boardX + (cellSize * 2)) {
                return 2;
            }
            return 3;
        }

        // second row
        if (y < boardY + (cellSize * 2)) {
            if (x < boardX + cellSize) {
                return 4;
            }
            if (x < boardX + (cellSize * 2)) {
                return 5;
            }
            return 6;
        }

        // third row
        if (y < boardY + cellSize * 3) {
            if (x < boardX + cellSize) {
                return 7;
            }
            if (x < boardX + (cellSize * 2)) {
                return 8;
            }
            return 9;
        }

        return -1;
    }

    void checkHasWinner() {
        if (((pieces[0].type == "X" && pieces[1].type == "X" && pieces[2].type == "X") ||
             (pieces[3].type == "X" && pieces[4].type == "X" && pieces[5].type == "X") ||
             (pieces[6].type == "X" && pieces[7].type == "X" && pieces[8].type == "X") ||
             (pieces[0].type == "X" && pieces[3].type == "X" && pieces[6].type == "X") ||
             (pieces[1].type == "X" && pieces[4].type == "X" && pieces[7].type == "X") ||
             (pieces[2].type == "X" && pieces[5].type == "X" && pieces[8].type == "X") ||
             (pieces[0].type == "X" && pieces[4].type == "X" && pieces[8].type == "X") ||
             (pieces[2].type == "X" && pieces[4].type == "X" && pieces[6].type == "X"))

            ||

            ((pieces[0].type == "O" && pieces[1].type == "O" && pieces[2].type == "O") ||
             (pieces[3].type == "O" && pieces[4].type == "O" && pieces[5].type == "O") ||
             (pieces[6].type == "O" && pieces[7].type == "O" && pieces[8].type == "O") ||
             (pieces[0].type == "O" && pieces[3].type == "O" && pieces[6].type == "O") ||
             (pieces[1].type == "O" && pieces[4].type == "O" && pieces[7].type == "O") ||
             (pieces[2].type == "O" && pieces[5].type == "O" && pieces[8].type == "O") ||
             (pieces[0].type == "O" && pieces[4].type == "O" && pieces[8].type == "O") ||
             (pieces[2].type == "O" && pieces[4].type == "O" && pieces[6].type == "O"))) {

            hasWinner = true;
        }
    }

    void AddPieceToCell(int cell) {
        for (auto &piece : pieces) {
            if (piece.cell == cell) {
                return;
            }
        }

        Piece piece;
        piece.type = xTurn ? "X" : "O";
        xTurn = !xTurn;

        piece.cell = cell;
        if (cell < 4) {
            piece.y = boardY;
            piece.x = boardX + (cellSize * (cell - 1)) + (cellSize / 4);
        }
        else if (cell < 7) {
            piece.y = boardY + cellSize;
            piece.x = boardX + (cellSize * (cell - 4)) + (cellSize / 4);
        }
        else {
            piece.y = boardY + (cellSize * 2);
            piece.x = boardX + (cellSize * (cell - 7)) + (cellSize / 4);
        }

        pieces[cell - 1] = piece;
    }

    void Render() {
        SDL_SetRenderDrawColor(renderer, 25, 25, 25, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        DrawTitle();
        DrawTicTacToeBoard();

        SDL_RenderPresent(renderer);
    }
}

int main(int, char **) {
    SDL_Init(SDL_INIT_VIDEO);
    if (TTF_Init() < 0) {
        std::cerr << "Error initializing TTF: " << TTF_GetError() << '\n';
        SDL_Quit();
        return -1;
    }

    pieceFont = TTF_OpenFont("./font.ttf", 150);
    titleFont = TTF_OpenFont("./font.ttf", 100);
    if (!pieceFont) {
        std::cerr << "Error loading font: " << TTF_GetError() << '\n';
        SDL_Quit();
        return -1;
    }

    window = SDL_CreateWindow("Tic Tac Toe",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              1000,
                              800,
                              SDL_WINDOW_OPENGL);
    if (NULL == window) {
        std::cout << "Failed to create window: " << SDL_GetError() << '\n';
        return -1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    bool isRunning{true};
    SDL_Event event;

    while (isRunning) {
        while (SDL_PollEvent(&event)) {
            if (SDL_QUIT == event.type) {
                SDL_DestroyWindow(window);
                SDL_DestroyRenderer(renderer);
                TTF_CloseFont(pieceFont);
                SDL_Quit();
                return 0;
            }
            if (SDL_MOUSEBUTTONDOWN == event.type) {
                if (hasWinner) {
                    reset();
                    continue;
                }
                int cell = GetCell(event.motion.x, event.motion.y);
                if (cell != -1) {
                    AddPieceToCell(cell);
                    checkHasWinner();
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        Render();
        SDL_RenderPresent(renderer);
    }

    return 0;
}
