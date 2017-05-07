/// \mainpage
/// \author Jan Věrný (xverny00)
/// \author Antonín Vlach (xvlach16)
/// \date 2017
/// \copyright University project

/// \file cli.cc
/// \author xvlach16
/// \brief Implementation of objects representing command line interface.

#include <iostream>
#include "objects.h"

using namespace std;
using namespace Hra2017;

namespace Hra2017
{
    /// \brief Class connecting game and command line interface.
    ///
    /// Carries information about the state of game and parsé and execute commands from standard input.
    class GameCLI
    {
        string command; ///< First word of input line, specifying which kind of action shall be done.
        std::vector<string> args; ///< Other words of input line, defining action more precisely.
        Game game; ///< Object carrying state of the game.
        Hint hint; ///< Carry infromation about recommended move and also about existence of any possible move.
        bool running = true; ///< Keeps running main loop in run method.

        /// \brief Converts string to unsignet integer.
        /// \param s String to be converted.
        /// \return Converted number or a negative value in case of failure.
        int stou(string s)
        {
            int ret{0};

            for (int i = 0; i < (int)s.length(); i++)
                if (s[i] >= '0' && s[i] <= '9')
                    ret = ret * 10 + s[i] - '0';
                else
                    return -1;

            return ret;
        }

        /// \brief Converts information about card to string.
        /// \param info Information to be converted.
        /// \return String describing the card (prepared for ouput).
        string writeCard(CardInfo info)
        {
            if (info.number == CardNumber::none)
                return string("   ");

            string numbers[14] {"  ", "A ", "2 ", "3 ", "4 ", "5 ", "6 ", "7 ", "8 ", "9 ", "10", "J ", "Q ", "K ",};
            string colors[4] {"H", "T", "C", "P"};

            if (info.hidden)
                return "X";
            else
                return numbers[info.number] + colors[info.color];
        }

        /// \brief Reads and parses command and its arguments from standard input.
        /// \return True if command was loaded successfully or false in other cases.
        bool loadCommand()
        {
            command.clear();
            args.clear();

            cout << ">>> " << flush;

            string s;
            if (!getline(cin, s))
            {
                cout << "End of stdin reached!\n";
                return false;
            }

            int idx{0};
            args.push_back("");
            for (int i = 0; i < (int)s.length(); i++)
                if (s[i] != ' ')
                    args[idx].push_back(s[i]);
                else if (args[idx] != "")
                {
                    idx++;
                    args.push_back("");
                }
            if (args.size() > 1 && args.back() == "")
                args.pop_back();

            command = args[0];
            args.erase(args.begin());

            return true;
        }

        /// \brief Writes detailed help to the standard output.
        ///
        /// Describes available commands and meaning of output.
        void writeHelp()
        {
            if (args.size() != 0)
            {
                cout << "Help: 0 arguments expected.\n";
                return;
            }

            cout << "Available commands:\n";
            cout << "    help - Write this help message.\n";
            cout << "    state - Write complete state of game.\n";
            cout << "    h - Short for hint.\n";
            cout << "    hint - Write hint of recommended move.\n";
            cout << "    a - Short for automove.\n";
            cout << "    automove - Write hint of recommended move.\n";
            cout << "    t - Short for turn.\n";
            cout << "    turn - Turn new card from stock to waste.\n";
            cout << "    get <dst> - Move card from waste to <dts> pile of tableau.\n";
            cout << "    move <src> <dst> - Move card on tableau from <src> pile to <dst> ptile.\n";
            cout << "    move <src> <dst> <n> - Move <n> cards on tableau from <src> pile to <dst> ptile.\n";
            cout << "    s[ <src>] - Short for submit.\n";
            cout << "    submit <src> - Move card from <src> pile of tableau to foundation.\n";
            cout << "    submit - Move card from waste to foundation.\n";
            cout << "    recover <src> <dst> - Move card from <src> pile of foundation to <dst> pile of tableau.\n";
            cout << "    u - Short for undo.\n";
            cout << "    undo - Undo last move.\n";
            cout << "    save <filename> - Save the game to <filename> file.\n";
            cout << "    exit - End this game and exit program.\n";
            cout << "\nCard representation:\n";
            cout << "    empty place: |   |\n";
            cout << "    back of card: |X|\n";
            cout << "    front of card: |<r> <c>|\n";
            cout << "        <n> rank of card; can be: A (ace), 2, 3, 4, 5, 6, 7, 8, 9, 10, J (jack), Q (queen), K (king)\n";
            cout << "        <c> color of card; can be: H (hearts), T (tiles/diamonds), C (clovers/clubs), P (pikes/spades)\n";
        }

        /// \brief Writes the current state of game.
        void writeState()
        {
            if (args.size() != 0)
            {
                cout << "State: 0 arguments expected.\n";
                return;
            }

            cout << "Stock: " << (game.isStockEmpty() ? "" : "not ") << "empty\n";

            vector<CardInfo> pile = game.getWaste();
            cout << "Waste: |" << (pile.empty() ? "   " : writeCard(pile.back())) << "|\n";
            
            cout << "Foundation: |";
            for (int i = 0; i < 4; i++)
            {
                pile = game.getFoundationPile(i);
                cout << (pile.empty() ? "   " : writeCard(pile.back())) << "|";
            }
            cout << endl;

            cout << "Tableau:\n";
            for (int i = 0; i < 7; i++)
            {
                cout << "|";
                pile = game.getTableauPile(i);
                for (int i2 = 0; i2 < (int)pile.size(); i2++)
                    cout << writeCard(pile[i2]) << "|";
                cout << endl;
            }

            cout << "Score: " << game.getScore() << endl;
        }

        /// \brief Writes recommended move.
        void writeHint()
        {
            if (args.size() != 0)
            {
                cout << "Hint: 0 arguments expected.\n";
                return;
            }

            if (hint.number == 0)
            {
                cout << "There are no hints.\n";
                return;
            }
            if (hint.destination == Pile::waste)
            {
                cout << "Turn new card from stock to waste (command: turn).\n";
                return;
            }

            string src;
            if (hint.source == Pile::waste)
                src = "waste";
            else if (hint.source <= Pile::tableau6)
                src = string("pile ") + to_string(hint.source) + " of tableau";
            else
                src = "foundation";

            string dst;
            if (hint.destination <= Pile::tableau6)
                dst = string("pile ") + to_string(hint.destination) + " of tableau";
            else
                dst = "foundation";

            string cmd;
            if (hint.source == Pile::waste && hint.destination <= Pile::tableau6)
                cmd = string("get ") + to_string(hint.destination);
            else if (hint.source == Pile::waste)
                cmd = "submit";
            else if (hint.destination > Pile::tableau6)
                cmd = string("submit ") + to_string(hint.source);
            else
                cmd = string("move ") + to_string(hint.source) + " " + to_string(hint.destination) + (hint.number == 1 ? "" : " " + to_string(hint.number));

            if (hint.number == 1)
                cout << "Move card from " << src << " to " << dst << " (command: " << cmd << ").\n";
            else
                cout << "Move " + to_string(hint.number) + " cards from " << src << " to " << dst << " (command: " << cmd << ").\n";
        }

        /// \brief Automaticaly does the move which would be recommendede by hint.
        void autoMove()
        {
            if (args.size() != 0)
            {
                cout << "Automove: 0 arguments expected.\n";
                return;
            }

            if (hint.number == 0)
            {
                cout << "There are no hints.\n";
                return;
            }
            if (hint.destination == Pile::waste)
            {
                cout << "turn\n";
                turn();
            }
            else if (hint.source == Pile::waste && hint.destination <= Pile::tableau6)
            {
                cout << "get " << hint.destination << endl;
                args.push_back(to_string(hint.destination));
                get();
            }
            else if (hint.source == Pile::waste)
            {
                cout << "submit\n";
                submit();
            }
            else if (hint.destination > Pile::tableau6)
            {
                cout << "submit " << hint.source << endl;
                args.push_back(to_string(hint.source));
                submit();
            }
            else
            {
                cout << "move " << hint.source << " " << hint.destination << (hint.number == 1 ? "" : " " + to_string(hint.number)) << endl;
                args.push_back(to_string(hint.source));
                args.push_back(to_string(hint.destination));
                args.push_back(to_string(hint.number));
                move();
            }
        }

        /// \brief Turns new card from stock to waste or recykle waste back to stock.
        void turn()
        {
            if (args.size() != 0)
            {
                cout << "Turn: 0 arguments expected.\n";
                return;
            }

            CardInfo info = game.turnNewCard();
            if (info.number == CardNumber::none)
            {
                if (game.isStockEmpty())
                    cout << "Stock and waste are empty.\n";
                else
                    cout << "Waste recycled to stock.\n";
            }
            else
            {
                cout << "New card: " << writeCard(info) << endl;

                if (game.isStockEmpty())
                    cout << "Stock is empty now.\n";
            }
        }

        /// \brief Moves new card from waste to tableau.
        void get()
        {
            if (args.size() != 1)
            {
                cout << "Get <dst>: 1 argument expected.\n";
                return;
            }

            int dst = stou(args[0]) - 1;
            if (dst < 0 || dst > 6)
            {
                cout << "Get <dst>: <dst> must be number between 1 and 7.\n";
                return;
            }

            int ret = game.moveFromWasteToTableau(dst);

            if (ret == 0)
                cout << "Done.\n";
            else if (ret == 1)
                cout << "Waste is empty.\n";
            else if (ret == 2)
                cout << "You can put the card only on card with rank one greater.\n";
            else if (ret == 3)
                cout << "You can put the card only on card with different color.\n";
            else if (ret == 4)
                cout << "You can put only a K (king) on the empty place.\n";
            else
                cout << "Something went wrong. (code " << ret << ")\n";
        }

        /// \brief Moves card(s) from tableau to another pile of tableau.
        void move()
        {
            if (args.size() < 2 || args.size() > 3)
            {
                cout << "Move <src> <dst>[ <n>]: 2 or 3 arguments expected.\n";
                return;
            }
            
            int src = stou(args[0]) - 1;
            int dst = stou(args[1]) - 1;
            int n = args.size() == 3 ? stou(args[2]) : 1;
            if (src < 0 || src > 6 || dst < 0 || dst > 6)
            {
                cout << "Move <src> <dst>[ <n>]: <src> and <dst> must be numbers between 1 and 7.\n";
                return;
            }
            if (n < 1)
            {
                cout << "Move <src> <dst> <n>: <n> must be a positive number.\n";
                return;
            }

            int ret = game.moveFromTableauToTableau(src, dst, n);

            if (ret == 0)
                cout << "Done.\n";
            else if (ret == 1)
                cout << "There is nothin to move.\n";
            else if (ret == 2)
                cout << "You can put the card only on card with rank one greater.\n";
            else if (ret == 3)
                cout << "You can put the card only on card with different color.\n";
            else if (ret == 4)
                cout << "You can put only a K (king) on the empty place.\n";
            else if (ret == 5)
                cout << "Too big number of cards to move.\n";
            else if (ret == 6)
                cout << "You cannot move hidden card.\n";
            else
                cout << "Something went wrong. (code " << ret << ")\n";
        }

        /// \brief Moves card from tableau or waste to foundation. 
        void submit()
        {
            if (args.size() > 1)
            {
                cout << "Submit[ <src>]: too much arguments.\n";
                return;
            }
            
            int ret;
            if (args.size() == 1)
            {
                int src = stou(args[0]) - 1;
                if (src < 0 || src > 6)
                {
                    cout << "Submit <src>: <src> must be number between 1 and 7.\n";
                    return;
                }

                vector<CardInfo> pile = game.getTableauPile(src);
                if (pile.size() == 0)
                {
                    cout << "There is nothin to move.\n";
                    return;
                }

                int dst = pile.back().color;
                ret = game.moveFromTableauToFoundation(src, dst);
            }
            else
            {
                vector<CardInfo> pile = game.getWaste();
                if (pile.size() == 0)
                {
                    cout << "There is nothin to move.\n";
                    return;
                }

                int dst = pile.back().color;
                ret = game.moveFromWasteToFoundation(dst);
            }

            if (ret == 0)
                cout << "Done.\n";
            else if (ret == 2)
                cout << "You can put the card only on card with rank one smaller.\n";
            else if (ret == 4)
                cout << "You can put only an A (ace) on the empty place of foundation.\n";
            else if (ret == 16)
            {
                cout << "VICTORY! Congratulations. Your score: " << game.getScore() << endl;
                running = false;
            }
            else
                cout << "Something went wrong. (code " << ret << ")\n";
        }

        /// \brief Moves card back from foundation to tableau.
        void recover()
        {
            if (args.size() != 2)
            {
                cout << "Recover <src> <dst>: 2 arguments expected.\n";
                return;
            }
            
            int src = stou(args[0]) - 1;
            int dst = stou(args[1]) - 1;
            if (src < 0 || src > 3)
            {
                cout << "Recover <src> <dst>: <src> must be number between 1 and 4.\n";
                return;
            }
            if (dst < 0 || dst > 6)
            {
                cout << "Recover <src> <dst>: <dst> must be number between 1 and 7.\n";
                return;
            }

            int ret = game.moveFromFoundationToTableau(src, dst);

            if (ret == 0)
                cout << "Done.\n";
            else if (ret == 1)
                cout << "There is nothin to move.\n";
            else if (ret == 2)
                cout << "You can put the card only on card with rank one greater.\n";
            else if (ret == 3)
                cout << "You can put the card only on card with different color.\n";
            else if (ret == 4)
                cout << "You can put only a K (king) on the empty place.\n";
            else
                cout << "Something went wrong. (code " << ret << ")\n";
        }
        
        /// \brief Undoes the last move.
        void undo()
        {
            if (args.size() != 0)
            {
                cout << "Undo: 0 arguments expected.\n";
                return;
            }

            if (game.undo())
                cout << "Undone.\n";
            else
                cout << "There is nothing to be undone.\n";
        }

        /// \brief Saves the game to a file.
        void save()
        {
            if (args.size() != 1)
            {
                cout << "Save: 1 argument expected.\n";
                return;
            }

            try
            {
                game.saveGame(args[0]);
                cout << "Game saved.\n";
            }
            catch (exception)
            {
                cout << "Error saving file.\n";
            }
        }
        
        /// \brief Asks and then (if confirmed) exits the program.
        void exitGame()
        {
            if (args.size() != 0)
            {
                cout << "Exit: 0 arguments expected.\n";
                return;
            }

            cout << "Are you sure you want to exit (y/N)? " << flush;
            string ans;
            getline(cin, ans);
            if (ans == "y" || ans == "Y")
                running = false;
        }

     public:
        /// \brief Constructs new game and shuffles randomly pack of cards and deals the new game.
        GameCLI() : game() { }

        /// \brief Loads saved game from file and deals the loaded game.
        /// \param filename Path (absolute or relative) to file with saved game.
        GameCLI(string filename) : game(filename) { }

        /// \brief Main loop of game.
        ///
        /// Reads and parses new command. And then executes it.
        /// Also checks if there still are some possible moves.
        void run()
        {
            writeState();
            while (running)
            {
                hint = game.getHint();
                if (hint.number == 0)
                {
                    cout << "GAME OVER! Sorry.\nType undo if you want to undo last move. " << flush;
                    if (!getline(cin, command))
                        break;
                    args.clear();
                    if (command == "undo")
                    {
                        undo();
                        continue;
                    }
                    break;
                }

                if (!loadCommand())
                    break;

                if (command == "")
                    continue;
                else if (command == "help")
                    writeHelp();
                else if (command == "state")
                    writeState();
                else if (command == "hint" || command == "h")
                    writeHint();
                else if (command == "automove" || command == "a")
                    autoMove();
                else if (command == "turn" || command == "t")
                    turn();
                else if (command == "get")
                    get();
                else if (command == "move")
                    move();
                else if (command == "submit" || command == "s")
                    submit();
                else if (command == "recover")
                    recover();
                else if (command == "undo" || command == "u")
                    undo();
                else if (command == "save")
                    save();
                else if (command == "exit")
                    exitGame();
                else
                    cout << "Unknown command \"" << command << "\". For available commands type help.\n";
            }
        }
    };
}

/// \brief Initialises and runs the game and runs it.
/// \param argc Count of command line arguments.
/// \param argv Command line arguments. If there is some, it is filename of saved game to be loaded.
/// \return 0 in case of succesfully ended program.
///         1 in case of bad command line argument or bad number of arguments.
///         2 in case of error while loading saved game.
int main(int argc, char *argv[])
{
    GameCLI *cli;

    if (argc == 1)
    {
        cli = new GameCLI();
        cout << "Game initialised. For available commands type help.\n";
    }
    else if (argc == 2)
    {
        if (string(argv[1]) == string("--help"))
        {
            cout << "Usage: hra2017-cli [saved-game-file]\n";
            cout << "Load and start game from saved-game-file or initialise a new one.\n";
            return 0;
        }

        try
        {
            cli = new GameCLI(argv[1]);
            cout << "Game loaded. For available commands type help.\n";
        }
        catch(const char *ex)
        {
            if (string(ex) == string("MISSING_DATA"))
                cerr << "Error loading file: Some data are missing in the file \"" << argv[1] << "\"!\n";
            else if (string(ex) == string("BAD_DATA"))
                cerr << "Error loading file: Data are in wrong format!\n";
            return 2;
        }
        catch(exception)
        {
            cerr << "Error loading file!\n";
            return 2;
        }
    }
    else
    {
        cerr << "Error: Too much arguments!\n";
        return 1;
    }

    cli->run();

    delete cli;
    return 0;
}