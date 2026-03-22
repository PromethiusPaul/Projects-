#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <vector>

struct Card {
    std::string rank;
    std::string suit;

    std::string toString() const {
        return rank + " of " + suit;
    }

    int value() const {
        if (rank == "A") return 1;
        if (rank == "J") return 11;
        if (rank == "Q") return 12;
        if (rank == "K") return 13;
        return std::stoi(rank);
    }
};

class SolitaireRNGGame {
private:
    std::vector<Card> deck;
    std::mt19937 rng;
    int score = 0;
    int roundsPlayed = 0;

    void buildDeck() {
        deck.clear();
        const std::vector<std::string> suits = {"Hearts", "Diamonds", "Clubs", "Spades"};
        const std::vector<std::string> ranks = {
            "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"
        };

        for (const auto& suit : suits) {
            for (const auto& rank : ranks) {
                deck.push_back({rank, suit});
            }
        }
    }

    void shuffleDeck() {
        std::shuffle(deck.begin(), deck.end(), rng);
    }

    void showMenu() const {
        std::cout << "\n=== Solitaire RNG Game ===\n";
        std::cout << "1. Draw a solitaire hand\n";
        std::cout << "2. Show rules\n";
        std::cout << "3. Reset game\n";
        std::cout << "4. Quit\n";
        std::cout << "Choose an option: ";
    }

    void showRules() const {
        std::cout << "\n--- Rules ---\n";
        std::cout << "This is a solitaire-themed random card draw game.\n";
        std::cout << "Each round draws 3 cards from a shuffled deck.\n";
        std::cout << "Scoring:\n";
        std::cout << "- Pair (same rank): +10 points\n";
        std::cout << "- Three of a kind: +30 points\n";
        std::cout << "- Straight (3 consecutive values): +20 points\n";
        std::cout << "- Flush (same suit): +15 points\n";
        std::cout << "- Mini-Solitaire Bonus: all black or all red suits: +12 points\n";
        std::cout << "- No combination: +0 points\n";
        std::cout << "Aces count as 1.\n";
    }

    bool sameColor(const Card& a, const Card& b) const {
        auto isRed = [](const std::string& suit) {
            return suit == "Hearts" || suit == "Diamonds";
        };
        return isRed(a.suit) == isRed(b.suit);
    }

    int evaluateHand(const std::vector<Card>& hand) const {
        std::vector<int> values;
        for (const auto& card : hand) {
            values.push_back(card.value());
        }
        std::sort(values.begin(), values.end());

        bool flush = (hand[0].suit == hand[1].suit && hand[1].suit == hand[2].suit);
        bool straight = (values[1] == values[0] + 1 && values[2] == values[1] + 1);
        bool pair = false;
        bool threeOfKind = false;

        if (hand[0].rank == hand[1].rank && hand[1].rank == hand[2].rank) {
            threeOfKind = true;
        } else if (hand[0].rank == hand[1].rank || hand[1].rank == hand[2].rank || hand[0].rank == hand[2].rank) {
            pair = true;
        }

        bool sameColorBonus = sameColor(hand[0], hand[1]) && sameColor(hand[1], hand[2]);

        int total = 0;
        if (threeOfKind) total += 30;
        else if (pair) total += 10;

        if (straight) total += 20;
        if (flush) total += 15;
        if (sameColorBonus) total += 12;

        return total;
    }

    void describeHand(const std::vector<Card>& hand, int points) const {
        std::cout << "\nYou drew:\n";
        for (const auto& card : hand) {
            std::cout << " - " << card.toString() << "\n";
        }

        std::vector<int> values;
        for (const auto& card : hand) values.push_back(card.value());
        std::sort(values.begin(), values.end());

        bool flush = (hand[0].suit == hand[1].suit && hand[1].suit == hand[2].suit);
        bool straight = (values[1] == values[0] + 1 && values[2] == values[1] + 1);
        bool threeOfKind = (hand[0].rank == hand[1].rank && hand[1].rank == hand[2].rank);
        bool pair = (!threeOfKind &&
            (hand[0].rank == hand[1].rank || hand[1].rank == hand[2].rank || hand[0].rank == hand[2].rank));
        bool sameColorBonus = sameColor(hand[0], hand[1]) && sameColor(hand[1], hand[2]);

        std::cout << "\nHand results:\n";
        if (threeOfKind) std::cout << " * Three of a kind (+30)\n";
        else if (pair) std::cout << " * Pair (+10)\n";
        if (straight) std::cout << " * Straight (+20)\n";
        if (flush) std::cout << " * Flush (+15)\n";
        if (sameColorBonus) std::cout << " * Mini-Solitaire Bonus (+12)\n";
        if (points == 0) std::cout << " * No scoring combination\n";

        std::cout << "Points this round: " << points << "\n";
    }

public:
    SolitaireRNGGame()
        : rng(static_cast<unsigned int>(
              std::chrono::steady_clock::now().time_since_epoch().count())) {
        buildDeck();
        shuffleDeck();
    }

    void reset() {
        score = 0;
        roundsPlayed = 0;
        buildDeck();
        shuffleDeck();
        std::cout << "\nGame reset.\n";
    }

    void drawRound() {
        if (deck.size() < 3) {
            std::cout << "\nDeck too small. Rebuilding and reshuffling deck...\n";
            buildDeck();
            shuffleDeck();
        }

        std::vector<Card> hand;
        for (int i = 0; i < 3; ++i) {
            hand.push_back(deck.back());
            deck.pop_back();
        }

        int points = evaluateHand(hand);
        score += points;
        roundsPlayed++;

        describeHand(hand, points);

        std::cout << "Total score: " << score << "\n";
        std::cout << "Rounds played: " << roundsPlayed << "\n";
        std::cout << "Cards remaining: " << deck.size() << "\n";
    }

    void run() {
        int choice = 0;

        while (true) {
            showMenu();

            if (!(std::cin >> choice)) {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                std::cout << "\nInvalid input. Please enter a number from 1 to 4.\n";
                continue;
            }

            switch (choice) {
                case 1:
                    drawRound();
                    break;
                case 2:
                    showRules();
                    break;
                case 3:
                    reset();
                    break;
                case 4:
                    std::cout << "\nThanks for playing Solitaire RNG.\n";
                    return;
                default:
                    std::cout << "\nPlease choose a valid option.\n";
                    break;
            }
        }
    }
};

int main() {
    SolitaireRNGGame game;
    game.run();
    return 0;
}
