#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <array>
#include <map>
#include <tuple>
#include <utility>
#include <bitset>
#include <algorithm>
#include <random>

typedef uint64_t U64;

// Empty bitboard.
constexpr U64 EMPTY_BITBOARD{ 0x00000000000000ULL };
const std::array<U64, 7> ALL_PIECES_EMPTY{ 0x00000000000000ULL, 0x00000000000000ULL, 0x00000000000000ULL,
0x00000000000000ULL, 0x00000000000000ULL, 0x00000000000000ULL, 0xFFFFFFFFFFFFFFFFULL };

// Starting bitboards.
const std::array<U64, 7> ALL_PIECES_START_POS{ 0xFF00000000FF00ULL, 0x4200000000000042ULL, 0x2400000000000024ULL,
0x8100000000000081ULL, 0x800000000000008ULL, 0x1000000000000010ULL, 0xFFFFFFFFFFFFFFFFULL };
constexpr U64 COLOR_START_POS{ 0xFFFF000000000000ULL };
constexpr U64 WHITE_PIECES_START_POS{ 0xFFFF000000000000ULL };
constexpr U64 BLACK_PIECES_START_POS{ 0xFFFFULL };

// Other starting data.
constexpr bool ACTIVE_COLOR_START_POS{ true };								// true - white, false - black.
const std::array<bool, 4> CASTLING_START_POS{ true, true, true, true };
const std::string EN_PASSANT_TARGET_START_POS{ "-" };
constexpr int HALFMOVE_CLOCK_START_POS{ 0 };
constexpr int FULLMOVE_NUMBER_START_POS{ 0 };

// Length of the part of the move string containing the coordinates of one square.
constexpr std::size_t LENGTH_ONE_SQUARE_COORDS{ 2 };

constexpr int LENGTH_IN_SQUARES_ONE_RANK{ 8 };
constexpr int ASCII_LOWER_CASE_A_INT{ 97 };
constexpr int ASCII_ZERO_INT{ 48 };

// Default for the player's color is zero-initialized.
constexpr bool PLAYER_COLOR_DEFAULT {};

#define set_bit(b, i) ((b) |= (1ULL << i))
#define get_bit(b, i) ((b) & (1ULL << i))
#define clear_bit(b, i) ((b) &= ~(1ULL << i))

#define all_pieces m_white_pieces | m_black_pieces
#define all_bitboards m_all_pieces_bitboards[0] | m_all_pieces_bitboards[1] | m_all_pieces_bitboards[2] | m_all_pieces_bitboards[3] | m_all_pieces_bitboards[4] | m_all_pieces_bitboards[5]
#define white_rooks_arr m_all_pieces_bitboards[3] & m_color
#define black_rooks_arr m_all_pieces_bitboards[3] & ~m_color


enum {
	a8, b8, c8, d8, e8, f8, g8, h8,
	a7, b7, c7, d7, e7, f7, g7, h7,
	a6, b6, c6, d6, e6, f6, g6, h6,
	a5, b5, c5, d5, e5, f5, g5, h5,
	a4, b4, c4, d4, e4, f4, g4, h4,
	a3, b3, c3, d3, e3, f3, g3, h3,
	a2, b2, c2, d2, e2, f2, g2, h2,
	a1, b1, c1, d1, e1, f1, g1, h1
};

/*
"a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
"a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
"a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
"a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
"a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
"a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
"a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
"a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
*/

std::string replace_digits_with_zeros(std::string fen);

// This is a struct containing the game data.
class GameData {

	// Bitboards for board data.
	std::array <U64, 7> m_all_pieces_bitboards {};
	U64 m_color {};
	U64 m_white_pieces {};
	U64 m_black_pieces {};

	// Values for additional data.
	bool m_active_color {};
	bool m_white_king_castling {};
	bool m_white_queen_castling {};
	bool m_black_king_castling {};
	bool m_black_queen_castling {};
	std::string m_en_passant_target {};
	int m_halfmove_clock {};
	int m_fullmove_number {};

	// Player's pieces color (1 - White, 0 - black)
	bool m_player_color{};

public:
	// Some parts will be removed !!!!!!!!!!!!!!!!!!!!!
	GameData(std::array<U64, 7> all_pieces_bitboards, U64 color, U64 white_pieces, U64 black_pieces, bool active_color, 
		std::array<bool, 4> castling_values, std::string en_passant_target, int halfmove_clock, int fullmove_number, 
		bool player_color)
		: m_all_pieces_bitboards { all_pieces_bitboards[0], all_pieces_bitboards[1], all_pieces_bitboards[2], 
		all_pieces_bitboards[3], all_pieces_bitboards[4], all_pieces_bitboards[5], all_pieces_bitboards[6] }
		, m_color { color }
		, m_white_pieces { white_pieces }
		, m_black_pieces { black_pieces }
		, m_active_color { active_color }
		, m_white_king_castling { castling_values[0] }
		, m_white_queen_castling { castling_values[1] }
		, m_black_king_castling { castling_values[2] }
		, m_black_queen_castling { castling_values[3] }
		, m_en_passant_target { en_passant_target }
		, m_halfmove_clock { halfmove_clock }
		, m_fullmove_number{ fullmove_number }
		, m_player_color{ player_color }
	{
	}

	// Function setting the board position from the FEN-string via the bitboards.
	void set_board_position(std::string board) {
		// Map connecting FEN string values with pointers to particular bitboards.
		std::map<std::string, U64*> fen_bit_arr{ {"P", &m_all_pieces_bitboards[0]}, {"N", &m_all_pieces_bitboards[1]}, 
		{"B", &m_all_pieces_bitboards[2]}, { "R", &m_all_pieces_bitboards[3] }, {"Q", &m_all_pieces_bitboards[4]}, 
		{ "K", &m_all_pieces_bitboards[5] }, {"p", &m_all_pieces_bitboards[0] }, { "n", &m_all_pieces_bitboards[1] },
		{ "b", &m_all_pieces_bitboards[2] }, { "r", &m_all_pieces_bitboards[3] }, {"q", &m_all_pieces_bitboards[4]}, 
		{ "k", &m_all_pieces_bitboards[5] }
		};
		for (std::size_t i = 0; i < board.length(); i++) {
			// If the character is not 0, we need to set the bit in the bitboard.
			if (board[i] != '0') {
				// Create a string from a character to be able to use a map.
				std::string s(1, board[i]);
				// Create a pointer to a particular bitboard that we need, according to the letter. 
				U64* pBitboard = fen_bit_arr[s];
				// Set that bit on that bitboard.
				set_bit(*pBitboard, i);
				// If the letter is uppercase, we also set a bit in the m_white_pieces bitboard.
				if (std::isupper(board[i])) {
					set_bit(m_white_pieces, i);
					set_bit(m_color, i);
				}
				// If the letter is lowercase, we set a bit in the m_black_pieces bitboard.
				else {
					set_bit(m_black_pieces, i);
				}
			}
		}
	}

	// This function writes white pieces positions into FEN string.
	void append_m_white_pieces_to_fen(std::string& fen, std::size_t bit) {
		// Write the appropriate letter (white pieces) into the future FEN string.
		if (get_bit(m_all_pieces_bitboards[0], bit))			fen[bit] = 'P';
		else if (get_bit(m_all_pieces_bitboards[1], bit))	    fen[bit] = 'N';
		else if (get_bit(m_all_pieces_bitboards[2], bit))  	    fen[bit] = 'B';
		else if (get_bit(m_all_pieces_bitboards[3], bit))		fen[bit] = 'R';
		else if (get_bit(m_all_pieces_bitboards[4], bit))	    fen[bit] = 'Q';
		else													fen[bit] = 'K';
	}

	// This function writes black pieces positions into FEN string.
	void append_m_black_pieces_to_fen(std::string& fen, std::size_t bit) {
		// Write the appropriate letter (black pieces) into the future FEN string.
		if (get_bit(m_all_pieces_bitboards[0], bit))          fen[bit] = 'p';
		else if (get_bit(m_all_pieces_bitboards[1], bit))     fen[bit] = 'n';
		else if (get_bit(m_all_pieces_bitboards[2], bit))     fen[bit] = 'b';
		else if (get_bit(m_all_pieces_bitboards[3], bit))     fen[bit] = 'r';
		else if (get_bit(m_all_pieces_bitboards[4], bit))     fen[bit] = 'q';
		else												  fen[bit] = 'k';
	}

	// This function writes pieces positions into FEN-to-be string.
	void append_pieces_to_fen(std::string& fen) {
		for (std::size_t bit = 0; bit < 64; bit++) {
			// Check bitboards for white and black pieces and call the function for the white/black accordingly.
			if (get_bit(m_white_pieces, bit))
				append_m_white_pieces_to_fen(fen, bit);
			else if (get_bit(m_black_pieces, bit))
				append_m_black_pieces_to_fen(fen, bit);
		}
	}

	//Is it ok to have this as a method? I think, logically it's a part of a class.
	// This function inserts slashes into the FEN string.
	void insert_slashes_to_fen(std::string& fen) {
		// Use a stringstream.
		std::stringstream ss;
		// Insert position zero outside the loop to prevent it from inserting a slash at the zero position.
		ss << fen[0];
		for (std::size_t i = 1; i < fen.length(); i++) {
			// If it's an eights position, insert a slash into the stringstream.
			if (i % 8 == 0) ss << '/';
			// Insert current letter into the stringstream.
			ss << fen[i];
		}
		// Assigning FEN string variable the value of the stringstream converted to a string.
		fen = ss.str();
	}

	// This function replaces zeros (that represent empty squares) with an appropriate number ("000" with 3 etc).
	void replace_zeros_with_digits(std::string& fen) {
		//Find first occurance of a zero in the string.
		std::size_t zero_found = fen.find_first_of("0");
		// Keep going until done.
		while (zero_found != std::string::npos) {
			std::size_t zero_idx = zero_found;
			// Check how many zeros in a row.
			while (fen[zero_idx] == '0') zero_idx++;
			// Index of a last zero minus index of a first found zero is the number of empty squares. 
			std::size_t num_empty_fields = zero_idx - zero_found;
			// Turn it into a string.
			std::string num_empty_fields_str = std::to_string(num_empty_fields);
			// Replacing zeros with a digit.
			fen.replace(zero_found, num_empty_fields, num_empty_fields_str);
			// Find next occurance of a zero.
			zero_found = fen.find_first_of("0", zero_found + 1);
		}
	}

	// This function appends all the other FEN data (except board position) to the FEN string.
	void append_other_data(std::string& fen) {
		fen.append(" ");
		std::map<bool, std::string> color_map{ {false, "b"}, {true, "w"} };
		fen.append(color_map[m_active_color]);
		fen.append(" ");
		if (m_white_king_castling == true)
			fen.append("K");
		if (m_white_queen_castling == true)
			fen.append("Q");
		if (m_black_king_castling == true)
			fen.append("k");
		if (m_black_queen_castling == true)
			fen.append("q");
		if (fen.back() == ' ')
			fen.append("-");
		fen.append(" ");
		if (m_en_passant_target == "")
			fen.append("-");
		else
			fen.append(m_en_passant_target);
		fen.append(" ");
		fen = fen + std::to_string(m_halfmove_clock);
		fen.append(" ");
		fen = fen + std::to_string(m_fullmove_number);
	}

	// This function writes all the data from the game object to the FEN string.
	void struct_to_fen() {
		// Create a string of 64 zeros for 64 positions on a board.
		std::string fen(64, '0');
		append_pieces_to_fen(fen);
		insert_slashes_to_fen(fen);
		// Now there are zeros left representing the empty fields. Turn those zeros into digits ("000" to 3 etc).
		replace_zeros_with_digits(fen);
		append_other_data(fen);
		std::cout << "Printing out the reconstructed fen: " << '\n';
		std::cout << fen << '\n';
	}

	// This function prints selected bitboard.
	// Functions for adding current board position to the FEN are going to use the same pattend (for every type of pieces).
	void print_bitboard(U64 bitboard) const {
		std::cout << "\n";
		for (int rank = 0; rank < 8; rank++) {
			for (int file = 0; file < 8; file++) {
				if (!file)
					std::cout << 8 - rank << ' ';
				int square = rank * 8 + file;
				// Print bit state (either 1 or 0).
				(get_bit(bitboard, square)) ? std::cout << 1 : std::cout << 0;
				std::cout << ' ';
			}
			std::cout << std::endl;
		}
		std::cout << "  a b c d e f g h" << "\n";
		std::cout << "\n";
		std::cout << "Bitboard:  " << +bitboard << "\n";
		std::cout << "\n";
	}

	// Function that prints all the pieces on the board.
	void print_the_board() const {
		print_bitboard(all_pieces);
	}

	// Function that prints different bitboards in the array.
	void print_bitboards() const {
		std::cout << "All bitboards in the array: " << '\n';
		print_bitboard(all_bitboards);
		std::cout << "White pieces: " << '\n';
		print_bitboard(m_white_pieces);
		std::cout << "Black pieces: " << '\n';
		print_bitboard(m_black_pieces);
	}

	// This function splits the UCI move string into a vector of 2 or 3 separate strings (2 for each square and 1 for 
	// promotion piece type if needed.
	std::vector<std::string> split_move(std::string move) {
		// Divide the length of the move string by the length of the string, containing coords. of one square (which is 2).
		std::size_t number_of_strings = move.length() / LENGTH_ONE_SQUARE_COORDS;
		// Create a vector of strings.
		std::vector<std::string> move_split{};
		// Add each substring of the move string containing one square coords. to the string vector. 
		for (std::size_t i = 0; i < number_of_strings; i++) {
			move_split.push_back(move.substr(i * LENGTH_ONE_SQUARE_COORDS, LENGTH_ONE_SQUARE_COORDS));
		}
		// If the length of the move string modulo 2 not equals 0 means that there was also a promotion type piece in the 
		// move string. Add it to the vector of strings as well.
		if (move.length() % LENGTH_ONE_SQUARE_COORDS != 0) {
			move_split.push_back(move.substr(LENGTH_ONE_SQUARE_COORDS * number_of_strings));
		}
		// Return the vector of strings, where two strings are move coords. and possibly one string is a promotion piece type.
		return move_split;
	}

	// This function takes each square coods. and returns the bit number of that square (for the bitboard).
	int string_to_bit(std::string square) {
		// To get int from char, containing file letter, we subtract integer ascii value of lowercase 'a' from it.
		int file = square[0] - ASCII_LOWER_CASE_A_INT;
		// To get int from char, containing rank number, we subtract integer ascii value of zero.
		int rank = LENGTH_IN_SQUARES_ONE_RANK - (square[1] - ASCII_ZERO_INT);
		// Create bit number variable. 
		int bit_number{};
		// To get the number of the bit, multiply rank number by 8 (cause 8 squares in the rank) and add file int value.
		bit_number = rank * LENGTH_IN_SQUARES_ONE_RANK + file;
		std::cout << bit_number<< " " << rank << " " << file << '\n';
		// Return the bit number.
		return bit_number;
	}

	// Function that gets bitboard that has a piece in a particular field.
	std::size_t get_bitboard(int move_from) {
		for (std::size_t i = 0;; ++i) {
			if (get_bit(m_all_pieces_bitboards[i], move_from)) {
				return i;
			}
		}
	}

	// This function makes a move on the bitboards.
	void make_a_move_bitboards(int move_from, int move_to, std::size_t bitboard_number_from, std::size_t bitboard_number_to) {
		// Replace with a constant.
		// Only need to clear a bit if there was a capture and it was not the same type of pieces (same type of pieces are
		// on the same bitboard).
		if ((bitboard_number_to < 6) && (bitboard_number_to != bitboard_number_from))
			clear_bit(m_all_pieces_bitboards[bitboard_number_to], move_to);
		set_bit(m_all_pieces_bitboards[bitboard_number_from], move_to);
		clear_bit(m_all_pieces_bitboards[bitboard_number_from], move_from);
		if (m_active_color == true) {
			clear_bit(m_black_pieces, move_to);
			set_bit(m_white_pieces, move_to);
			clear_bit(m_white_pieces, move_from);
			set_bit(m_color, move_to);
			clear_bit(m_color, move_from);
		}
		else {
			clear_bit(m_color, move_to);
			set_bit(m_black_pieces, move_to);
			clear_bit(m_black_pieces, move_from);
		}
	}

	// This function converts move string to 2 integers representing "move from" and "move to" positions on the bitboards.
	std::tuple<int, int> move_string_to_int(std::string move) {
		std::cout << move << '\n';
		// Split the move string into an array of strings ("from" square, "to" square, and, if needed, promotion piece type).
		std::vector<std::string> move_split{ split_move(move) };
		std::cout << "From: " << move_split[0] << " To: " << move_split[1] << '\n';
		// Convert strings representing each square into int bit number on the bitboard.
		int move_from = string_to_bit(move_split[0]);
		int move_to = string_to_bit(move_split[1]);
		std::cout << "From: " << move_from << " To: " << move_to << '\n';
		// Return the number of the bitboard that has a piece on that position.
		return std::tuple <int, int> (move_from, move_to);
	}

	// This function makes a move on all bitboards.
	void make_a_move(int move_from, int move_to) {
		std::size_t bitboard_number_from = get_bitboard(move_from);
		// If bitboard number is equal 6, we reached the sentinel value, meaning there is no piece on the "from" square on 
		// any of the bitboards. Throw an exception and report an error. 
		try {
			if (bitboard_number_from == 6)
				throw "there is no bitboard with a piece in that square.";
		}
		catch (const char* exception) {
			std::cerr << "Error: " << exception << '\n';
		}
		std::size_t bitboard_number_to = get_bitboard(move_to);
		make_a_move_bitboards(move_from, move_to, bitboard_number_from, bitboard_number_to);
		m_active_color = !m_active_color;
		std::cout << "Active color is: " << m_active_color << '\n';
	}

	// This function sets player's pieces color
	void set_player_color(std::string pl_color) {
		// Set the color depending on the input (w or b); Otherwise report an error.
		if (pl_color == "w")                m_player_color = true;
		else if (pl_color == "b")           m_player_color = false;
		else								std::cout << "Wrong input!" << '\n';

		std::cout << "Player color is: " << m_player_color << '\n';
	}

	// This function make the player's move. It returns 0 if the correct move was entered, it returns 1 if 0 was entered to
	// stop the game. 
	int make_players_move(std::string move) {
		std::cout << "Please, enter the next move: ";
		std::getline(std::cin, move);
		if (move == "0") return 1;
		int move_from{};
		int move_to{};
		std::tie(move_from, move_to) = move_string_to_int(move);
		make_a_move(move_from, move_to);
		return 0;
	}

	std::tuple <int, int> generate_random_move_comp () {
		std::vector<int> comp_square_numbers = get_comp_square_numbers();
		std::cout << "All white/black pieces: " << '\n';
		for (int x : comp_square_numbers)
			std::cout << x << ' ';
		std::cout << '\n';
		int random_move_from = get_random_square(comp_square_numbers);
		// Choose "move to" from empty squares and opposition pieces randomly.
		std::vector<int> pl_and_empty_square_numbers = get_pl_and_empty_square_numbers();
		std::cout << "All player pieces and empty squares: " << '\n';
		for (int x : pl_and_empty_square_numbers)
			std::cout << x << ' ';
		std::cout << '\n';
		// Choose a random square from all the empty squares and squares with opponent's pieces on them.
		int random_move_to = get_random_square(pl_and_empty_square_numbers);
		std::cout << "Comp move: " << random_move_from << ' ' << random_move_to << '\n';
		return std::tuple <int, int>(random_move_from, random_move_to);
	}



	// Function that gets positions of all computer pieces.
	std::vector<int> get_comp_square_numbers() const {
		std::vector<int> comp_square_numbers;
		// If player is playing black pieces, get positions of all white pieces (computer's pieces). 
		if (m_player_color == 0) {
			for (int i = 0; i < 64; ++i) {
				if (get_bit(m_white_pieces, i)) {
					comp_square_numbers.push_back(i);
				}
			}
		}
		// If player is playing white pieces, get positions of all black pieces for the comp. 
		else {
			for (int i = 0; i < 64; ++i) {
				if (get_bit(m_black_pieces, i)) {
					comp_square_numbers.push_back(i);
				}
			}
		}
		return comp_square_numbers;
	}

	// Function that gets positions of all player pieces and empty squares depending on what computer is playing.
	std::vector<int> get_pl_and_empty_square_numbers() const {
		std::vector<int> comp_square_numbers;
		// If player is playing black pieces, get positions of all white pieces (computer's pieces). 
		if (m_player_color == 0) {
			for (int i = 0; i < 64; ++i) {
				if (!get_bit(m_white_pieces, i)) {
					comp_square_numbers.push_back(i);
				}
			}
		}
		// If player is playing white pieces, get positions of all black pieces for the comp. 
		else {
			for (int i = 0; i < 64; ++i) {
				if (!get_bit(m_black_pieces, i)) {
					comp_square_numbers.push_back(i);
				}
			}
		}
		return comp_square_numbers;
	}

	// Function to get a random square from the vector of squares.
	int get_random_square(std::vector<int> squares_numbers) {
		// Vector for keeping the chosen random square.
		std::vector<int> random_move_from;
		// Number of random elements required.
		size_t num_elems{ 1 };
		// Choose one random element from the vector containing squares with pieces on them.
		// https://stackoverflow.com/questions/6942273/how-to-get-a-random-element-from-a-c-container/42484107#42484107
		std::sample(squares_numbers.begin(), squares_numbers.end(), std::back_inserter(random_move_from),
			num_elems, std::mt19937{ std::random_device{}() });
		return random_move_from[0];
	}

	// This function represents a game loop.
	void game_loop() {
		std::string move{};
		while (true) {
			// Player's move if active color matches his color.
			if (m_active_color == m_player_color) {
				// Make player's move. Zero check checks if the input was "0", which stops the game loop.
				int zero_check = make_players_move(move);
				if (zero_check == 1) break;
				print_the_board();
				print_bitboards();
			}
			// Otherwise it's computer's move.
			else {
				int random_move_from{};															// "move from" coord.
				int random_move_to{};															// "move to" coord.
				// Generate random move for computer.
				std::tie(random_move_from, random_move_to) = generate_random_move_comp();
				// Make a move.
				make_a_move(random_move_from, random_move_to);
				print_the_board();
				print_bitboards();
			}
		}
	}
};

// This function prints a human-readable ascii board representation.
//void print_board_ascii(FenData& game) {
//	std::map<int, std::string> fen_map{ {-4, "r"}, {-3, "b"}, {-2, "n"}, {-1, "p"}, {-5, "q"}, {-6, "k"},
//	{4, "R"}, {3, "B"}, {2, "N"}, {1, "P"}, {5, "Q"}, {6, "K"}, {0, "."} };
//	for (std::size_t i = 2; i < game.board.size() - 2; i++)
//	{
//		for (std::size_t j = 2; j < game.board[i].size() - 2; j++)
//		{
//			std::cout << fen_map[game.board[i][j]];
//		}
//		std::cout << std::endl;
//	}
//}

// Is it possible to move the loop into a different func?
// This function returns black pawn moves.
//std::vector <std::pair<size_t, size_t>> get_black_pawn_moves (FenData& game) {
//	std::vector <std::pair<size_t, size_t>> black_pawn_moves = {};
//	for (std::size_t i = 2; i < game.board.size() - 2; i++)
//	{
//		for (std::size_t j = 2; j < game.board[i].size() - 2; j++)
//		{
//			if (i == 3 && game.board[i][j] == -1 && game.board[i + 2][j] == 0)
//				black_pawn_moves.push_back(std::make_pair(i + 2, j));
//			if (game.board[i][j] == -1 && game.board[i + 1][j] == 0)
//				black_pawn_moves.push_back(std::make_pair(i + 1, j));
//			if (game.board[i][j] == -1 && game.board[i + 1][j - 1] > 0 && game.board[i + 1][j - 1] < 7)
//				black_pawn_moves.push_back(std::make_pair(i + 1, j - 1));
//			if (game.board[i][j] == -1 && game.board[i + 1][j + 1] > 0 && game.board[i + 1][j + 1] < 7)
//				black_pawn_moves.push_back(std::make_pair(i + 1, j + 1));
//		}
//	}
//	return black_pawn_moves;
//}

// This function returns white pawn moves.
//std::vector <std::pair<size_t, size_t>> get_white_pawn_moves(FenData& game) {
//	std::vector <std::pair<size_t, size_t>> white_pawn_moves = {};
//	for (std::size_t i = 2; i < game.board.size() - 2; i++)
//	{
//		for (std::size_t j = 2; j < game.board[i].size() - 2; j++)
//		{
//			if (i == 8 && game.board[i][j] == 1 && game.board[i - 2][j] == 0)
//				white_pawn_moves.push_back(std::make_pair(i - 2, j));
//			if (game.board[i][j] == 1 && game.board[i - 1][j] == 0)
//				white_pawn_moves.push_back(std::make_pair(i - 1, j));
//			if (game.board[i][j] == 1 && game.board[i - 1][j - 1] > 0 && game.board[i - 1][j - 1] < 7)
//				white_pawn_moves.push_back(std::make_pair(i - 1, j - 1));
//			if (game.board[i][j] == 1 && game.board[i - 1][j + 1] > 0 && game.board[i - 1][j + 1] < 7)
//				white_pawn_moves.push_back(std::make_pair(i - 1, j + 1));
//		}
//	}
//	return white_pawn_moves;
//}

//This fuction replaces digits in the FEN with a number of zeros (1 with 1 zero, 2 - with 2 zeros, 3 - 3 zeros etc)
std::string replace_digits_with_zeros(std::string board) {
	//Find first occurance of digits in the FEN-string.
	std::size_t found = board.find_first_of("0123456789");
	// Keep going until done.
	while (found != std::string::npos) {
		// This turns the digit char into an integer.
		int empty_squares_digit = board[found] - '0';
		// Turning the integer into size_t to be able to create a string with it (this will be the length of the string).
		// (!!!!!) Need an error-check function here (!!!!!)
		// Check if it's positive.
		std::size_t zeros_length = static_cast<unsigned int>(empty_squares_digit);
		// Creating a string with appropriate number of zeros.
		std::string zeros(zeros_length, '0');
		// Replacing a digit with an appropriate number of zeros.
		board.replace(found, 1, zeros);
		// Find next occurance of any of the digits.
		found = board.find_first_of("0123456789", found + zeros_length);
	}
	return board;
}

// This function returns castling values from the current FEN-string.
std::array<bool, 4> get_castling_values(std::string fen_castling, std::array<bool, 4> castling_values) {
	// Castling values are initialized to false, so if we see a particular castling value letter in a string, the
	// appropriate value is set to true. 
	for (char& castling_char : fen_castling) {
		switch (castling_char) {
		case 'K':
			castling_values[0] = true;
			break;
		case 'k':
			castling_values[1] = true;
			break;
		case 'Q':
			castling_values[2] = true;
			break;
		case 'q':
			castling_values[3] = true;
			break;
		}
	}
	return castling_values;
}

// This function returns all the data from the FEN-string.
std::tuple<std::string, std::array<bool, 4>, bool, std::string, int, int> extract_values_from_fen(std::string fen, 
	std::string board, std::array<bool, 4> castling_values, bool active_color, std::string en_passant_target, 
	int halfmove_clock, int fullmove_number) {

	// Turn the string into istringstream to extract data.
	std::istringstream ssfen(fen);

	// Create two string variables to hold castling and active color strings for future use.
	std::string castling_string{};
	std::string active_color_str{};

	// Extract all the data from the istringstream into the variables.
	ssfen >> board >> active_color_str >> castling_string >> en_passant_target >> halfmove_clock >> fullmove_number;
	// Use a map to set active color bool value from active color string.
	std::map<std::string, bool> color_map{ {"b", false}, {"w", true} };
	active_color = color_map[active_color_str];
	// Use function to get castling values from castling value string.
	castling_values = get_castling_values(castling_string, castling_values);
	// Remove slashes and replace all digits with appropriate number of zeros (3 becomes "000" etc) to be able to 
	// loop through the string and put values directly to the bitboards.
	board.erase(std::remove(board.begin(), board.end(), '/'), board.end());
	board = replace_digits_with_zeros(board);

	// Return all the extracted data.
	return std::make_tuple(board, castling_values, active_color, en_passant_target, halfmove_clock, fullmove_number);
}

// Function that creates game object from the FEN string.
GameData create_game_object_from_fen(std::string fen) {

	// Zero - initialize all the variables we are going to use for FEN data except:
	// castling values are init. to false due to the design of the get_castling_values func.
	std::string board{};
	std::array<bool, 4> castling_values = { false, false, false, false };
	bool active_color{};
	std::string en_passant_target{};
	int halfmove_clock{};
	int fullmove_number{};

	// Get the values for those variables using extract_values_from_fen func.
	std::tie(board, castling_values, active_color, en_passant_target, halfmove_clock, fullmove_number) = extract_values_from_fen(fen,
		board, castling_values, active_color, en_passant_target, halfmove_clock, fullmove_number);

	// Create a game object setting all bitboards to empty (to be filled in the set_board_position func. with the values from
	// the FEN) and the data that we extracted from the FEN.
	GameData gameData{ ALL_PIECES_EMPTY, EMPTY_BITBOARD, EMPTY_BITBOARD, EMPTY_BITBOARD, active_color, 
					   castling_values, en_passant_target, halfmove_clock, fullmove_number, PLAYER_COLOR_DEFAULT };
	// Set bitboards according to the board position part of the FEN.
	gameData.set_board_position(board);

	// Return game object created using FEN string.
	return gameData;
}

// This function creates game object for the starting position.
GameData create_game_object_start_pos() {

	// Create game object using starting position constants.
	GameData gameData{ ALL_PIECES_START_POS, COLOR_START_POS, WHITE_PIECES_START_POS, BLACK_PIECES_START_POS, 
					   ACTIVE_COLOR_START_POS, CASTLING_START_POS, EN_PASSANT_TARGET_START_POS, HALFMOVE_CLOCK_START_POS, 
					   FULLMOVE_NUMBER_START_POS, PLAYER_COLOR_DEFAULT };

	// Return game object with the starting position.
	return gameData;
}

int main()
{
	std::string fen{};
	// U64 test{ ~uint64_t(0) };
	std::cout << "Please, enter the FEN or press enter to start the game from the beginning: ";
	std::getline(std::cin, fen);
	// Add "pl_color" value to the game class (1 - white, 0 - black);
	GameData gameData = fen.length() > 5 ? create_game_object_from_fen(fen) : create_game_object_start_pos();
	std::string pl_color{};
	std::cout << "Please, enter w to choose white and b to choose black pieces: ";
	std::getline(std::cin, pl_color);
	gameData.set_player_color(pl_color);
	std::cout << "All pieces:" << '\n';
	gameData.print_the_board();
	if (fen.length() > 5) {
		std::cout << "FEN is: " << fen << std::endl;
	}
	gameData.game_loop();
	//std::vector <std::pair<size_t, size_t>> piece_moves = {};
	//print_board_ascii(game);
	//print_board_ascii(game);
	//piece_moves = get_black_pawn_moves(game); 
	//std::vector <std::pair<size_t, size_t>> white_pawn_moves = get_white_pawn_moves(game);
	//piece_moves.insert(std::end(piece_moves), std::begin(white_pawn_moves), std::end(white_pawn_moves));
	//for (std::pair<size_t, size_t> pawn_move : piece_moves)
	//	std::cout << pawn_move.first << pawn_move.second << std::endl;
	gameData.struct_to_fen();
	return 0;
}
