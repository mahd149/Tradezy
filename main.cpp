
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include "C:\Users\MSI\Desktop\final\Raylib-CPP-Starter-Template-for-VSCODE-V2-main\bg2.h"

#define NUM_COMPANIES 5
#define INITIAL_SHARES 0
#define MAX_TRANSACTIONS 500

double portfolio[31];
static int days = 1;




typedef enum
{
    MAIN_MENU,
    VIEW_PORTFOLIO,
    VIEW_PRICES,    
    SELL_SHARES,
    VIEW_TRANSACTIONS,
    SELECT_COMPANY_TO_BUY,
    CONFIRM_BUY,
    SELECT_COMPANY_TO_SELL,
    CONFIRM_SELL,  
    
    GAME_OVER,
    SHOW_GRAPH,
    GRAPH_A,
    GRAPH_B,
    GRAPH_C,
    GRAPH_D,
    GRAPH_E,
    GRAPH_PORTFOL

} GameState;

typedef struct
{
    char name[20];
    int shares;
    double price;
} Company;

typedef struct
{
    int shares[NUM_COMPANIES];
    double portfolio_value;
    double last_transaction_prices[MAX_TRANSACTIONS];
    int last_transaction_companies[MAX_TRANSACTIONS];
    int last_transaction_shares[MAX_TRANSACTIONS];
    int num_transactions;
    double initial_portfolio_value;
    double total_worth;
} Trader;

void initializeCompanies(Company companies[], double arrForCompanyPrices[][31]);
void initializeTrader(Trader *trader);
void printPortfolio(Company companies[], Trader *trader);
void buyShares(Company companies[], Trader *trader, int company_index, int num_shares);
void sellShares(Company companies[], Trader *trader, int company_index, int num_shares);
void displayTransactions(Company companies[], Trader *trader);
double generateRandomPrice(double previousPrice);
void GameOver(Company companies[], Trader *trader);
void ViewPrices(Company companies[], Trader *trader);
void totalWorth(Company companies[], Trader *trader);
void drawMainMenu();
void drawPortfolio(Company companies[], Trader *trader);
void drawSharePrices(Company companies[]);
void showgraphs(Company companies[], Trader *trader);
void selectCompanyToBuy(Company companies[], Trader *trader);
void confirmBuy(Company companies[], Trader *trader, int company_index);
void selectCompanyToSell(Company companies[], Trader *trader);
void confirmSell(Company companies[], Trader *trader, int company_index);

void makeStaticGraphA(FILE *fa, FILE *gnuplot_pipe);
void makeStaticGraphB(FILE *fb, FILE *gnuplot_pipe);
void makeStaticGraphC(FILE *fc, FILE *gnuplot_pipe);
void makeStaticGraphD(FILE *fd, FILE *gnuplot_pipe);
void makeStaticGraphE(FILE *fe, FILE *gnuplot_pipe);
void Portfolio_graph(FILE *portfol, FILE *gnuplot_pipe);

void makeDataFile(double arrForCompanyPrices[5][31], double *portfolio, FILE *fa, FILE *fb, FILE *fc, FILE *fd, FILE *fe, FILE *portfol);
void makeStaticGraphALL(FILE *fa, FILE *fb, FILE *fc, FILE *fd, FILE *fe, FILE *gnuplot_pipe);

void initGNUPLOTandFILES(FILE *fa, FILE *fb, FILE *fc, FILE *fd, FILE *fe, FILE *portfol, FILE *gnuplot_pipe);

bool Button(Rectangle rec, const char *text);
bool quit_button(Rectangle rec, const char *text);

GameState gameState = MAIN_MENU;
int selectedCompany = -1;
char numSharesInput[10] = {0}; // Buffer to store the input as a string
int numSharesInputIndex = 0;   // Current position in the input buffer

Texture2D gamebg ;




int main()
{
    Company companies[NUM_COMPANIES];
    Trader trader;

    FILE *gnuplot_pipe = NULL;

    gnuplot_pipe = _popen("gnuplot -persist", "w");
    // pipeline pointer to gnuplot, dont need to understand much
    if (gnuplot_pipe == NULL)
    {
        printf("failed to initialize gnuplot");
    }

    FILE *fa = fopen("A.dat", "w");
    FILE *fb = fopen("B.dat", "w");
    FILE *fc = fopen("C.dat", "w");
    FILE *fd = fopen("D.dat", "w");
    FILE *fe = fopen("E.dat", "w");

    FILE *portfol = fopen("Portfol.dat", "w");

    if (fa == NULL)
    {
        printf("Error opening file for A.dat\n");
    }
    if (fb == NULL)
    {
        printf("Error opening file for B.Dat\n");
    }
    if (fc == NULL)
    {
        printf("Error opening file for C.dat\n");
    }
    if (fd == NULL)
    {
        printf("Error opening file for D.dat\n");
    }
    if (fe == NULL)
    {
        printf("Error opening file for E.dat\n");
    }

    double arrForCompanyPrices[5][31] = {0};

    Color emerald = {27, 148, 97, 255};

    

    initializeCompanies(companies, arrForCompanyPrices);
    initializeTrader(&trader);

    srand(time(NULL));
    

    // Initialize Raylib
    InitWindow(800, 600, "Tradezy");
    SetTargetFPS(60);

    Image bg2 = {0};
    bg2.format = BG2_FORMAT;
    bg2.height = BG2_HEIGHT;
    bg2.width = BG2_WIDTH;
    bg2.data = BG2_DATA;
    bg2.mipmaps = 1;

    gamebg = LoadTextureFromImage(bg2);

    
    


    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTexture(gamebg,0,0, WHITE);

        switch (gameState)
        {
        case MAIN_MENU:
            drawMainMenu();
            
            DrawText(TextFormat("Day: %2d ", days), 150, 120, 30, emerald);
            if (Button((Rectangle){150, 460, 225, 75}, "End the day"))
            {

                for (int i = 0; i < 5; i++)
                {
                    companies[i].price = generateRandomPrice(companies[i].price);
                }
                days++;
                for (int i = 0; i < 5; i++)
                {
                    for (int j = (days - 1); j < days; j++)
                    {
                        arrForCompanyPrices[i][j] = companies[i].price;
                    }
                }
                for (int i = (days - 1); i < days; i++)
                {
                    totalWorth(companies, &trader);
                    portfolio[i] = trader.total_worth;
                }
                printf("\n\nGood Night zzz ;)");
                if (days <= 30)
                {
                    printf("\n\n\n******* Current day: %d *******\n", days);
                }
                if (days >= 31)
                {
                    printf("\n\n\nTIMES UP!\n");

                    gameState = GAME_OVER; // Reset to main menu after game over

                    initGNUPLOTandFILES(fa, fb, fc, fd, fe, portfol, gnuplot_pipe);
                }
                makeDataFile(arrForCompanyPrices, portfolio, fa, fb, fc, fd, fe, portfol);
                makeStaticGraphALL(fa, fb, fc, fd, fe, gnuplot_pipe);
            }
            if (quit_button((Rectangle){450, 460, 225, 75}, "Quit"))
            {
                printf("\n\nExiting...\n\n");

                gameState = GAME_OVER;
            }
            break;

        case VIEW_PORTFOLIO:
            drawPortfolio(companies, &trader);
            if (Button((Rectangle){20, 520, 150, 50}, "Go Back"))
            {
                gameState = MAIN_MENU;
            }
            break;

        case VIEW_PRICES:
            drawSharePrices(companies);
            if (Button((Rectangle){20, 520, 150, 50}, "Go Back"))
            {
                gameState = MAIN_MENU;
            }
            break;

        case SELECT_COMPANY_TO_BUY:
            selectCompanyToBuy(companies, &trader);
            if (Button((Rectangle){20, 520, 150, 50}, "Go Back"))
            {
                gameState = MAIN_MENU;
            }
            break;

        case CONFIRM_BUY:
            confirmBuy(companies, &trader, selectedCompany);
            if (Button((Rectangle){20, 520, 150, 50}, "Go Back"))
            {
                gameState = MAIN_MENU;
            }
            break;

        case SELECT_COMPANY_TO_SELL:
            selectCompanyToSell(companies, &trader);
            if (Button((Rectangle){20, 520, 150, 50}, "Go Back"))
            {
                gameState = MAIN_MENU;
            }
            break;

        case CONFIRM_SELL:
            confirmSell(companies, &trader, selectedCompany);
            if (Button((Rectangle){20, 520, 150, 50}, "Go Back"))
            {
                gameState = MAIN_MENU;
            }
            break;

        case SELL_SHARES:
            sellShares(companies, &trader, selectedCompany, atoi(numSharesInput));
            gameState = MAIN_MENU; // Return to main menu after selling shares
            break;

        case VIEW_TRANSACTIONS:
            displayTransactions(companies, &trader);
            if (Button((Rectangle){20, 520, 150, 50}, "Go Back"))
            {
                gameState = MAIN_MENU;
            }
            break;
        // case INVALID_SELL:
        //     // DrawText(TextFormat("Ivalid operation"), 20, 40, 20, BLACK);
        //     gameState = MAIN_MENU;
        case SHOW_GRAPH:
            showgraphs(companies, &trader);
            if (Button((Rectangle){20, 520, 150, 50}, "Go Back"))
            {
                gameState = MAIN_MENU;
            }
            break;
        case GRAPH_A:
            makeStaticGraphA(fa, gnuplot_pipe);
            selectedCompany = -1;
            gameState = SHOW_GRAPH;
            break;
        case GRAPH_B:
            makeStaticGraphB(fb, gnuplot_pipe);
            selectedCompany = -1;
            gameState = SHOW_GRAPH;
            break;
        case GRAPH_C:
            makeStaticGraphC(fc, gnuplot_pipe);
            selectedCompany = -1;
            gameState = SHOW_GRAPH;
            break;
        case GRAPH_D:
            makeStaticGraphD(fd, gnuplot_pipe);
            selectedCompany = -1;
            gameState = SHOW_GRAPH;
            break;
        case GRAPH_E:
            makeStaticGraphE(fe, gnuplot_pipe);
            selectedCompany = -1;
            gameState = SHOW_GRAPH;
            break;
        case GRAPH_PORTFOL:

            break;
        case GAME_OVER:
            GameOver(companies, &trader);
            if (Button((Rectangle){20, 520, 210, 50}, "PLAY AGAIN"))
            {
                days = 1;
                for (int i = 0; i < 5; i++)
                {
                    for (int j = 0; j < 31; j++)
                    {
                        arrForCompanyPrices[i][j] = 0;
                    }
                }

                initializeCompanies(companies, arrForCompanyPrices);
                initializeTrader(&trader);

                initGNUPLOTandFILES(fa, fb, fc, fd, fe, portfol, gnuplot_pipe);
                gameState = MAIN_MENU;
            }
            break;
        }

        EndDrawing();
    }

    CloseWindow();
    // UnloadTexture(gamebg);
    fclose(fa);
    fclose(fb);
    fclose(fc);
    fclose(fd);
    fclose(fe);
    fclose(portfol);
    pclose(gnuplot_pipe);

    return 0;
}

bool Button(Rectangle rec, const char *text)
{
    Vector2 mousePoint = GetMousePosition();
    bool pressed = false;
    Color emerald = {27, 148, 97, 255};
    Color maroon = { 190, 33, 55, 255 };
    

    if (CheckCollisionPointRec(mousePoint, rec))
    {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            DrawRectangleRec(rec, maroon);
            // DrawText(text, rec.x + 10, rec.y + 10, 30, WHITE);
        }
        else
        {
            DrawRectangleRec(rec, emerald);
        }

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        {
            pressed = true;
        }
    }
    else
    {
        DrawRectangleRec(rec, LIGHTGRAY);
    }
    DrawText(text, rec.x + 10, rec.y + 10, 30, BLACK);

    
    return pressed;
}


bool quit_button(Rectangle rec, const char *text)
{
    Vector2 mousePoint = GetMousePosition();
    bool pressed = false;
    
    Color maroon = { 190, 33, 55, 255 };

    if (CheckCollisionPointRec(mousePoint, rec))
    {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            DrawRectangleRec(rec, GRAY);
        }
        else
        {
            DrawRectangleRec(rec, maroon);
        }

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        {
            pressed = true;
        }
    }
    else
    {
        DrawRectangleRec(rec, LIGHTGRAY);
    }

    DrawText(text, rec.x + 10, rec.y + 10, 30, BLACK);
    return pressed;
}


void drawMainMenu()
{
    // DrawTexture(gamebg,0,0, WHITE);
    // Color maroon = { 190, 33, 55, 255 };
    // DrawText("V_Mad", 600,550,20, maroon);
    // DrawText("Tradezy", 300, 20, 40, BLACK);
    // DrawTexture(gamebg,0,0, WHITE);
    
    

    // DrawText("Each company has issued 100 shares during their initial public offering...", 20, 60, 20, BLACK);
    // DrawText("You have 30 days to become rich or go broke", 200, 100, 20, BLACK);

    if (Button((Rectangle){150, 160, 225, 75}, "Portfolio"))
    {
        gameState = VIEW_PORTFOLIO;
    }
    if (Button((Rectangle){450, 160, 225, 75}, "Share Prices"))
    {
        gameState = VIEW_PRICES;
    }
    if (Button((Rectangle){150, 260, 225, 75}, "Buy Shares"))
    {
        gameState = SELECT_COMPANY_TO_BUY;
    }
    if (Button((Rectangle){450, 260, 225, 75}, "Sell Shares"))
    {
        gameState = SELECT_COMPANY_TO_SELL;
    }
    if (Button((Rectangle){150, 360, 225, 75}, "Transactions"))
    {
        gameState = VIEW_TRANSACTIONS;
    }
    if (Button((Rectangle){450, 360, 225, 75}, "View Graphs"))
    {
        gameState = SHOW_GRAPH;
    }
}

void displayTransactions(Company companies[], Trader *trader)
{
    ClearBackground(RAYWHITE);
    Color maroon = { 190, 33, 55, 255 };
    
    Color emerald = {27, 148, 97, 255};
    // DrawText(TextFormat("Latest Transactions:\n"), 20, 20, 40, BLACK);
    int yPos = 120;
    if (trader->num_transactions < 10)
    {
        for (int i = 0; i < trader->num_transactions; i++)
        {
            if (trader->last_transaction_shares[i] >= 0)
            {
                char type = trader->last_transaction_shares[i] > 0 ? 'B' : 'S';
                int shares = abs(trader->last_transaction_shares[i]);
                DrawText(TextFormat("%c: %d shares of %s at %.2f $ each", type, shares, companies[trader->last_transaction_companies[i]].name, trader->last_transaction_prices[i]), 20, yPos, 30, maroon);
                yPos += 40;
            }
            else{
                char type = trader->last_transaction_shares[i] > 0 ? 'B' : 'S';
                int shares = abs(trader->last_transaction_shares[i]);
                DrawText(TextFormat("%c: %d shares of %s at %.2f $ each", type, shares, companies[trader->last_transaction_companies[i]].name, trader->last_transaction_prices[i]), 20, yPos, 30,emerald );
                yPos += 40;
            }
        }
    }
    else
    {
       for (int i = trader->num_transactions-10; i < trader->num_transactions; i++)
        {
            if (trader->last_transaction_shares[i] >= 0)
            {
                char type = trader->last_transaction_shares[i] > 0 ? 'B' : 'S';
                int shares = abs(trader->last_transaction_shares[i]);
                DrawText(TextFormat("%c: %d shares of %s at %.2f $ each", type, shares, companies[trader->last_transaction_companies[i]].name, trader->last_transaction_prices[i]), 20, yPos, 30, maroon);
                yPos += 40;
            }
            else{
                char type = trader->last_transaction_shares[i] > 0 ? 'B' : 'S';
                int shares = abs(trader->last_transaction_shares[i]);
                DrawText(TextFormat("%c: %d shares of %s at %.2f $ each", type, shares, companies[trader->last_transaction_companies[i]].name, trader->last_transaction_prices[i]), 20, yPos, 30,emerald );
                yPos += 40;
            }
        }
    }
}
void drawPortfolio(Company companies[], Trader *trader)
{
    
   
    ClearBackground(RAYWHITE);
    //   DrawTexture(gamebg,0,0,WHITE);
   
    
   
    // DrawText("Portfolio:", 20, 0, 40, emerald);
    int yPos = 120;
    for (int i = 0; i < NUM_COMPANIES; i++)
    {
        DrawText(TextFormat("%s: %3d shares, Current Price: %7.2lf", companies[i].name, trader->shares[i], companies[i].price), 20, yPos, 30, BLACK);
        yPos += 50;
    }
    yPos += 50;
    DrawText(TextFormat("Buying Capacity:            %.2lf $", trader->portfolio_value), 20, yPos, 30, BLACK);
    yPos += 50;
    totalWorth(companies, trader);
    DrawText(TextFormat("Portfolio:                     %.2lf $", trader->total_worth), 20, yPos, 30, BLACK);
}

void drawSharePrices(Company companies[])
{
    int yPos = 20;
    
   
    // DrawText("Current Share Prices:", 20, yPos, 40, BLACK);
    yPos += 100;
    for (int i = 0; i < NUM_COMPANIES; i++)
    {
        DrawText(TextFormat("%s Share Price: %7.2f $", companies[i].name, companies[i].price), 20, yPos, 30, BLACK);
        yPos += 50;
    }
}

void showgraphs(Company companies[], Trader *trader)
{
    ClearBackground(RAYWHITE);
    
    
    int yPos = 20;
    // DrawText("Select a graph to be displayed:", 20, yPos, 20, BLACK);
    yPos += 80;
    for (int i = 0; i < 5; i++)
    {
        if (Button((Rectangle){20, yPos, 300, 40}, TextFormat("%s", companies[i].name )))
        {
            selectedCompany = i;
        }
        yPos += 50;
    }
    if (selectedCompany == 0)
    {
        gameState = GRAPH_A;
    }
    else if (selectedCompany == 1)
    {
        gameState = GRAPH_B;
    }
    else if (selectedCompany == 2)
    {
        gameState = GRAPH_C;
    }
    else if (selectedCompany == 3)
    {
        gameState = GRAPH_D;
    }
    else if (selectedCompany == 4)
    {
        gameState = GRAPH_E;
    }
    else
    {
        return;
    }
}

void selectCompanyToBuy(Company companies[], Trader *trader)
{
    ClearBackground(RAYWHITE);
    
    
    int yPos = 20;
    // DrawText("Select a company to buy shares from:", 20, yPos, 20, BLACK);
    yPos += 130;
    for (int i = 0; i < NUM_COMPANIES; i++)
    {
        if (Button((Rectangle){20, yPos, 760, 40}, TextFormat("%s - at %7.2f $   Available Shares %3d", companies[i].name, companies[i].price, companies[i].shares)))
        {
            selectedCompany = i;
            gameState = CONFIRM_BUY;
        }
        yPos += 50;
    }
}

void confirmBuy(Company companies[], Trader *trader, int company_index)
{
    Color emerald = {27, 148, 97, 255};
    
    
    DrawText(TextFormat("%s:", companies[company_index].name), 20, 150, 30, BLACK);
    DrawText(TextFormat("Current Price: %7.2f $", companies[company_index].price), 20, 190, 30, BLACK);
    DrawText(TextFormat("Available Shares: %3d", companies[company_index].shares), 20, 230, 30, BLACK);
    DrawText(TextFormat("Buying Capacity: %.2lf $", trader->portfolio_value), 20, 270, 30, BLACK);
    DrawText("Enter number of shares to buy:", 20, 310, 30, emerald);

    // Display the current input
    DrawText(numSharesInput, 20, 350, 30, emerald);

    int key = GetKeyPressed();
    if (key >= '0' && key <= '9' && (numSharesInputIndex < sizeof(numSharesInput) - 1))
    {
        numSharesInput[numSharesInputIndex] = (char)key;
        numSharesInputIndex++;
        numSharesInput[numSharesInputIndex] = '\0'; // Null-terminate the string
    }

    if (Button((Rectangle){20, 390, 200, 50}, "Confirm Buy"))
    {
        int numSharesToBuy = atoi(numSharesInput); // Convert the input to an integer
        buyShares(companies, trader, company_index, numSharesToBuy);
        numSharesInputIndex = 0;  // Reset the input index
        numSharesInput[0] = '\0'; // Clear the input buffer
        gameState = SELECT_COMPANY_TO_BUY;
    }
}

void selectCompanyToSell(Company companies[], Trader *trader)
{
    ClearBackground(RAYWHITE);
    
    
    int yPos = 150;
    // DrawText("Select a company to sell shares from:", 20, yPos, 20, BLACK);
    // yPos += 40;
    for (int i = 0; i < NUM_COMPANIES; i++)
    {
        if (Button((Rectangle){20, yPos, 600, 40}, TextFormat("%s - sold at %7.2f $ each", companies[i].name, companies[i].price)))
        {
            selectedCompany = i;
            gameState = CONFIRM_SELL;
        }
        yPos += 50;
    }
}

void confirmSell(Company companies[], Trader *trader, int company_index)
{
    Color emerald = {27, 148, 97, 255};
    
    
    DrawText(TextFormat("%s:", companies[company_index].name), 20, 150, 30, BLACK);
    DrawText(TextFormat("Current Price: %7.2f $", companies[company_index].price), 20, 190, 30, BLACK);
    DrawText(TextFormat("Your Shares: %d", trader->shares[company_index]), 20, 230, 30, BLACK);
    DrawText("Enter number of shares to sell:", 20, 270, 30, emerald);

    // Display the current input
    DrawText(numSharesInput, 20, 310, 30, emerald);

    int key = GetKeyPressed();
    if (key >= '0' && key <= '9' && numSharesInputIndex < sizeof(numSharesInput) - 1)
    {
        numSharesInput[numSharesInputIndex] = (char)key;
        numSharesInputIndex++;
        numSharesInput[numSharesInputIndex] = '\0'; // Null-terminate the string
    }

    if (Button((Rectangle){20, 350, 200, 50}, "Confirm Sell"))
    {
        int numSharesToSell = atoi(numSharesInput); // Convert the input to an integer
        sellShares(companies, trader, company_index, numSharesToSell);
        numSharesInputIndex = 0;  // Reset the input index
        numSharesInput[0] = '\0'; // Clear the input buffer
        gameState = SELECT_COMPANY_TO_SELL;
    }
}

void initializeCompanies(Company companies[], double arrForCompanyPrices[][31])
{
    strcpy(companies[0].name, "Tradezy ");
    strcpy(companies[1].name, "AjkeChuti");
    strcpy(companies[2].name, "ByteME  ");
    strcpy(companies[3].name, "CDS East");
    strcpy(companies[4].name, "CDS West");
    companies[4].price = 90.0;
    companies[3].price = 120.0;
    companies[2].price = 75.0;
    companies[1].price = 50.0;
    companies[0].price = 100.0;

    for (int i = 0; i < NUM_COMPANIES; i++)
    {
        companies[i].shares = 100; // Each company starts with 100 shares
        // companies[i].price = 10.0; // Initial price for all companies
    }
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 1; j++)
        {
            arrForCompanyPrices[i][j] = companies[i].price;
        }
    }
}

void initializeTrader(Trader *trader)
{
    for (int i = 0; i < NUM_COMPANIES; i++)
    {
        trader->shares[i] = INITIAL_SHARES;
    }
    trader->portfolio_value = 10000.0;
    trader->num_transactions = 0;
    trader->initial_portfolio_value = 10000.0;
    trader->total_worth = 10000.0;
    portfolio[0] = 10000;
}

void buyShares(Company companies[], Trader *trader, int company_index, int num_shares)
{
    if (num_shares > companies[company_index].shares)
    {
        // gameState = INVALID_SELL;
        return;
    }
    double total_cost = num_shares * companies[company_index].price;

    if (total_cost > trader->portfolio_value)
    {
        // gameState = INVALID_SELL;
        return;
    }

    trader->shares[company_index] += num_shares;
    companies[company_index].shares -= num_shares;
    trader->portfolio_value -= total_cost;

    trader->last_transaction_prices[trader->num_transactions] = companies[company_index].price;
    trader->last_transaction_companies[trader->num_transactions] = company_index;
    trader->last_transaction_shares[trader->num_transactions] = num_shares;
    trader->num_transactions++;

    // updatePortfolioValue(companies, trader);
    printf("Bought %d shares of %s at %.2f each.\n", num_shares, companies[company_index].name, companies[company_index].price);
}

void sellShares(Company companies[], Trader *trader, int company_index, int num_shares)
{
    if (num_shares > trader->shares[company_index])
    {
        // printf("Not enough shares available to sell.\n");
        return;
    }

    trader->shares[company_index] -= num_shares;
    companies[company_index].shares += num_shares;
    double total_income = num_shares * companies[company_index].price;
    trader->portfolio_value += total_income;

    trader->last_transaction_prices[trader->num_transactions] = companies[company_index].price;
    trader->last_transaction_companies[trader->num_transactions] = company_index;
    trader->last_transaction_shares[trader->num_transactions] = -num_shares; // Negative for sell
    trader->num_transactions++;

    // updatePortfolioValue(companies, trader);
    printf("Sold %d shares of %s at %.2f each.\n", num_shares, companies[company_index].name, companies[company_index].price);
}



double generateRandomPrice(double previousPrice)
{
    double minChange = -10.0; // Minimum price change
    double maxChange = 10.0;  // Maximum price change
    double change = ((double)rand() / RAND_MAX) * (maxChange - minChange) + minChange;
    double newPrice = previousPrice + change;
    if (newPrice < 1.0)
        newPrice = 1.0; // Minimum price is 1.0
    return newPrice;
}

void GameOver(Company companies[], Trader *trader)
{
    Color emerald = {27, 148, 97, 255};
    Color maroon = { 190, 33, 55, 255 };
    
    ClearBackground(RAYWHITE);
    DrawText(TextFormat("GAME OVER!!"), 20, 120, 30, emerald);
    int ypos = 120;
    // DrawText(TextFormat("Final Portfolio:\n"), 20, ypos, 30, BLACK);
    ypos += 50;
    for (int i = 0; i < NUM_COMPANIES; i++)
    {
        DrawText(TextFormat("%s: %d shares\n", companies[i].name, trader->shares[i]), 20, ypos, 30, BLACK);
        ypos += 50;
    }
    // DrawText(TextFormat("Remaining dosh: %.2f $", trader->portfolio_value), 20, ypos, 20, BLACK);
    ypos += 30;
    totalWorth(companies, trader);
    DrawText(TextFormat("Portfolio: %.2lf$", trader->total_worth), 20, ypos, 30, BLACK);
    if(trader->total_worth > trader->initial_portfolio_value)
    {
        DrawText("This is how we roll", 600,550,20, maroon);
    }
    else if (trader->total_worth<trader->initial_portfolio_value)
    {
        DrawText("Gotta Pay up", 600,550,20, maroon);
    }
    else
    {
        DrawText("Standing on the edge", 550,550,20, maroon);
    }
}

void totalWorth(Company companies[], Trader *trader)
{
    trader->total_worth = trader->portfolio_value;
    for (int i = 0; i < NUM_COMPANIES; i++)
    {
        trader->total_worth += trader->shares[i] * companies[i].price;
    }
}

void makeDataFile(double arrForCompanyPrices[5][31], double *portfolio, FILE *fa, FILE *fb, FILE *fc, FILE *fd, FILE *fe, FILE *portfol)
{

    for (int i = 0; i < days; i++)
    {
        for (int j = 0; j <= i; j++)
        {
            fprintf(fa, "%d %lf\n", (j + 1), arrForCompanyPrices[0][j]);
        }
        fprintf(fa, "\n\n");
    }

    for (int i = 0; i < days; i++)
    {
        for (int j = 0; j <= i; j++)
        {
            fprintf(fb, "%d %lf\n", (j + 1), arrForCompanyPrices[1][j]);
        }
        fprintf(fb, "\n\n");
    }
    for (int i = 0; i < days; i++)
    {
        for (int j = 0; j <= i; j++)
        {
            fprintf(fc, "%d %lf\n", (j + 1), arrForCompanyPrices[2][j]);
        }
        fprintf(fc, "\n\n");
    }
    for (int i = 0; i < days; i++)
    {
        for (int j = 0; j <= i; j++)
        {
            fprintf(fd, "%d %lf\n", (j + 1), arrForCompanyPrices[3][j]);
        }
        fprintf(fd, "\n\n");
    }
    for (int i = 0; i < days; i++)
    {
        for (int j = 0; j <= i; j++)
        {
            fprintf(fe, "%d %lf\n", (j + 1), arrForCompanyPrices[4][j]);
        }
        fprintf(fe, "\n\n");
    }
    for (int i = 0; i < days; i++)
    {
        for (int j = 0; j <= i; j++)
        {
            fprintf(portfol, "%d %lf\n", (j + 1), portfolio[j]);
        }
        fprintf(portfol, "\n\n");
    }

    fflush(fa);
    fflush(fb);
    fflush(fc);
    fflush(fd);
    fflush(fe);
    fflush(portfol);
}

void initGNUPLOTandFILES(FILE *fa, FILE *fb, FILE *fc, FILE *fd, FILE *fe, FILE *portfol, FILE *gnuplot_pipe)
{

    gnuplot_pipe = NULL;

    gnuplot_pipe = _popen("gnuplot -persist", "w");
    // pipeline pointer to gnuplot, dont need to understand much
    if (gnuplot_pipe == NULL)
    {
        printf("failed to initialize gnuplot");
    }

    fa = fopen("A.dat", "w");
    fb = fopen("B.dat", "w");
    fc = fopen("C.dat", "w");
    fd = fopen("D.dat", "w");
    fe = fopen("E.dat", "w");

    portfol = fopen("Portfol.dat", "w");

    if (fa == NULL)
    {
        printf("Error opening file for A.dat\n");
    }
    if (fb == NULL)
    {
        printf("Error opening file for B.Dat\n");
    }
    if (fc == NULL)
    {
        printf("Error opening file for C.dat\n");
    }
    if (fd == NULL)
    {
        printf("Error opening file for D.dat\n");
    }
    if (fe == NULL)
    {
        printf("Error opening file for E.dat\n");
    }
}

void makeStaticGraphALL(FILE *fa, FILE *fb, FILE *fc, FILE *fd, FILE *fe, FILE *gnuplot_pipe)

{

    fprintf(gnuplot_pipe, "set title \"Tradezy\" font \"Garamond,20\" \n");

    fprintf(gnuplot_pipe, "set grid\n");

    fprintf(gnuplot_pipe, "set xtics 0,2,%d\n", 40);
    fprintf(gnuplot_pipe, "set tics nomirror\n");
    fprintf(gnuplot_pipe, "set tics scale 1.5\n");

    fprintf(gnuplot_pipe, "set xlabel \"Days\" font \"Garamond, 18\" \n");
    fprintf(gnuplot_pipe, "set ylabel \" Share Price\" font \"Garamond, 20\"\n");
    fprintf(gnuplot_pipe, "set ylabel offset 0\n");
    fprintf(gnuplot_pipe, "set xrange [1:30]\n");

    fprintf(gnuplot_pipe, "set border 3\n");

    fprintf(gnuplot_pipe, "set key width 1\n");
    fprintf(gnuplot_pipe, "set key box\n");
    fprintf(gnuplot_pipe, "set key font \"Arial,12\"\n");
    fprintf(gnuplot_pipe, " plot \"A.dat\" w l lc 7 lw 1  title \"Tradezy \", \"B.dat\" w l lc 3 lw 1  title \"AjkeChuti\" , \"C.dat\" w l lc 18 lw 1  title \"ByteME\" ,  \"D.dat\" w l lc 13 lw 1 title \"CDS East\", \"E.dat\" w l lc 17 lw 1  title \"CDS West\"  \n");

    fflush(gnuplot_pipe);
}

void makeStaticGraphA(FILE *fa, FILE *gnuplot_pipe)
{

    fprintf(gnuplot_pipe, "set title \"Tradezy\" font \"Garamond,20\" \n");

    fprintf(gnuplot_pipe, "set grid\n");
    // fprintf(gnuplot_pipe, "stats \"A.dat\" name \"A\"\n");

    fprintf(gnuplot_pipe, "set xtics 0,5,%d\n", 40);
    fprintf(gnuplot_pipe, "set tics nomirror\n");
    fprintf(gnuplot_pipe, "set tics scale 1.5\n");
    fprintf(gnuplot_pipe, "set xrange [1:31]\n");

    fprintf(gnuplot_pipe, "set xlabel \"Days\" font \"Garamond, 18\" \n");
    fprintf(gnuplot_pipe, "set ylabel \" Share Price\" font \"Garamond, 20\"\n");
    fprintf(gnuplot_pipe, "set ylabel offset 0\n");

    fprintf(gnuplot_pipe, "set border 3\n");

    fprintf(gnuplot_pipe, "set key width 1\n");
    fprintf(gnuplot_pipe, "set key box\n");
    fprintf(gnuplot_pipe, "set key font \"Arial,12\"\n");

    fprintf(gnuplot_pipe, " plot \"A.dat\" using 1:2:(sprintf(\"%%d$\",  $2)) with labels point pt 7 offset char 2,1 notitle, \"A.dat\" w lines lc 7 lw 1 title \"Tradezy \"\n");
    fflush(gnuplot_pipe);
}
void makeStaticGraphB(FILE *fb, FILE *gnuplot_pipe)
{

    fprintf(gnuplot_pipe, "set title \"Tradezy\" font \"Garamond,20\" \n");

    fprintf(gnuplot_pipe, "set grid\n");
    fprintf(gnuplot_pipe, "set xrange [1:31]\n");

    fprintf(gnuplot_pipe, "set xtics 0,5,%d\n", 40);
    fprintf(gnuplot_pipe, "set tics nomirror\n");
    fprintf(gnuplot_pipe, "set tics scale 1.5\n");

    fprintf(gnuplot_pipe, "set xlabel \"Days\" font \"Garamond, 18\" \n");
    fprintf(gnuplot_pipe, "set ylabel \" Share Price\" font \"Garamond, 20\"\n");
    fprintf(gnuplot_pipe, "set ylabel offset 0\n");

    fprintf(gnuplot_pipe, "set border 3\n");

    fprintf(gnuplot_pipe, "set key width 1\n");
    fprintf(gnuplot_pipe, "set key box\n");
    fprintf(gnuplot_pipe, "set key font \"Arial,12\"\n");

    fprintf(gnuplot_pipe, " plot \"B.dat\" using 1:2:(sprintf(\"%%d$\",  $2)) with labels point pt 7 offset char 2,1 notitle, \"B.dat\" w lines lc 7 lw 1 title \"AjkeChuti\"\n");
    fflush(gnuplot_pipe);
}
void makeStaticGraphC(FILE *fc, FILE *gnuplot_pipe)
{

    fprintf(gnuplot_pipe, "set title \"Tradezy\" font \"Garamond,20\" \n");

    fprintf(gnuplot_pipe, "set grid\n");

    fprintf(gnuplot_pipe, "set xtics 0,5,%d\n", 40);
    fprintf(gnuplot_pipe, "set tics nomirror\n");
    fprintf(gnuplot_pipe, "set tics scale 1.5\n");

    fprintf(gnuplot_pipe, "set xlabel \"Days\" font \"Garamond, 18\" \n");
    fprintf(gnuplot_pipe, "set ylabel \" Share Price\" font \"Garamond, 20\"\n");
    fprintf(gnuplot_pipe, "set ylabel offset 0\n");
    fprintf(gnuplot_pipe, "set xrange [1:31]\n");

    fprintf(gnuplot_pipe, "set border 3\n");

    fprintf(gnuplot_pipe, "set key width 1\n");
    fprintf(gnuplot_pipe, "set key box\n");
    fprintf(gnuplot_pipe, "set key font \"Arial,12\"\n");

    fprintf(gnuplot_pipe, " plot \"C.dat\" using 1:2:(sprintf(\"%%d$\",  $2)) with labels point pt 7 offset char 2,1 notitle, \"C.dat\" w lines lc 7 lw 1 title \"ByteME\"\n");
    fflush(gnuplot_pipe);
}

void makeStaticGraphD(FILE *fd, FILE *gnuplot_pipe)
{

    fprintf(gnuplot_pipe, "set title \"Tradezy\" font \"Garamond,20\" \n");

    fprintf(gnuplot_pipe, "set grid\n");

    fprintf(gnuplot_pipe, "set xtics 0,5,%d\n", 40);
    fprintf(gnuplot_pipe, "set tics nomirror\n");
    fprintf(gnuplot_pipe, "set tics scale 1.5\n");

    fprintf(gnuplot_pipe, "set xlabel \"Days\" font \"Garamond, 18\" \n");
    fprintf(gnuplot_pipe, "set ylabel \" Share Price\" font \"Garamond, 20\"\n");
    fprintf(gnuplot_pipe, "set ylabel offset 0\n");

    fprintf(gnuplot_pipe, "set border 3\n");
    fprintf(gnuplot_pipe, "set xrange [1:31]\n");

    fprintf(gnuplot_pipe, "set key width 1\n");
    fprintf(gnuplot_pipe, "set key box\n");
    fprintf(gnuplot_pipe, "set key font \"Arial,12\"\n");

    fprintf(gnuplot_pipe, " plot \"D.dat\" using 1:2:(sprintf(\"%%d$\",  $2)) with labels point pt 7 offset char 2,1 notitle, \"D.dat\" w lines lc 7 lw 1 title \"CDS East\"\n");
    fflush(gnuplot_pipe);
}

void makeStaticGraphE(FILE *fe, FILE *gnuplot_pipe)
{

    fprintf(gnuplot_pipe, "set title \"Tradezy\" font \"Garamond,20\" \n");

    fprintf(gnuplot_pipe, "set grid\n");

    fprintf(gnuplot_pipe, "set xtics 0,5,%d\n", 40);
    fprintf(gnuplot_pipe, "set tics nomirror\n");
    fprintf(gnuplot_pipe, "set tics scale 1.5\n");

    fprintf(gnuplot_pipe, "set xlabel \"Days\" font \"Garamond, 18\" \n");
    fprintf(gnuplot_pipe, "set ylabel \" Share Price\" font \"Garamond, 20\"\n");
    fprintf(gnuplot_pipe, "set ylabel offset 0\n");
    fprintf(gnuplot_pipe, "set xrange [1:31]\n");

    fprintf(gnuplot_pipe, "set border 3\n");

    fprintf(gnuplot_pipe, "set key width 1\n");
    fprintf(gnuplot_pipe, "set key box\n");
    fprintf(gnuplot_pipe, "set key font \"Arial,12\"\n");

    fprintf(gnuplot_pipe, " plot \"E.dat\" using 1:2:(sprintf(\"%%d$\",  $2)) with labels point pt 7 offset char 2,1 notitle, \"E.dat\" w lines lc 7 lw 1 title \"CDS West\"\n");
    fflush(gnuplot_pipe);
}

void Portfolio_graph(FILE *portfol, FILE *gnuplot_pipe)
{
    fprintf(gnuplot_pipe, "set title \"Tradezy\" font \"Garamond,20\" \n");

    fprintf(gnuplot_pipe, "set grid\n");

    fprintf(gnuplot_pipe, "set xtics 0,5,%d\n", 40);
    fprintf(gnuplot_pipe, "set tics nomirror\n");
    fprintf(gnuplot_pipe, "set tics scale 1.5\n");

    fprintf(gnuplot_pipe, "set xlabel \"Days\" font \"Garamond, 18\" \n");
    fprintf(gnuplot_pipe, "set ylabel \" Share Price\" font \"Garamond, 20\"\n");
    fprintf(gnuplot_pipe, "set ylabel offset 0\n");

    fprintf(gnuplot_pipe, "set border 3\n");

    fprintf(gnuplot_pipe, "set key width 1\n");
    fprintf(gnuplot_pipe, "set key box\n");
    fprintf(gnuplot_pipe, "set key font \"Arial,12\"\n");

    fprintf(gnuplot_pipe, " plot \"Portfol.dat\" using 1:2:(sprintf(\"%%d$\",  $2)) with labels point pt 7 offset char 2,1 notitle, \"Portfol.dat\" w lines lc 7 lw 1 title \"Portfolio\"\n");
    fflush(gnuplot_pipe);
}