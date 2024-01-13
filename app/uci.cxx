#include <iostream>
#include "print.hpp"
#include "lut.hpp"
#include "position.hpp"
#include "gen_move.hpp"
#include "move_maker.hpp"
#include "eval.hpp"

unsigned VERBOSE = 1;


void PrintExtMoveForUCI(ExtMove extmv){
    Color c = Position::sideToMove;
	// Special treatment: the mate in is signaled
	// by the bits 16-20, with all other bits set to 0.
	// In addition if there is mate in x for BLACK
	// the 31th bit is set
	int is31thSet = extmv.value & (1ULL<<31);
	int mateInMultiplier = is31thSet? -1:1;
	// clear 31th bit
	extmv.value = extmv.value*mateInMultiplier;

	// Special treatement 2
	// in UCI the score is relative to who plays
	// So I have to add a multiplication factor depending on who plays
	// ColorScoreFactor[c]
	// MATE IN is not affected by this thing
	
	
	
    if((extmv.value == VALUE_MATE_0))
        std::cout<<" score mate 0";
    else{
        auto string_first = mvhuman(extmv.move);
        MoveType mt = MoveType( extmv.move & 0xC000);


        if((extmv.value  >= VALUE_MATE_30) & (extmv.value  < VALUE_MATE_0)){
            int mateInPlys = (VALUE_MATE_0 - extmv.value) >> 16;
            std::cout<<" score mate "<<mateInMultiplier*mateInPlys;
        }

        else if((extmv.value == VALUE_NONE))
            std::cout<<string_first<< " "<<"\tscore NONE\n";
        else if((abs(extmv.value) == VALUE_INFINITE))
            std::cout<<string_first<< " "<<"\tscore INFINITE\n";
        // returns the score as from the point of view.
        // so if it is white playing the score is correct
        // if it is black playing the score has to be inverted
        else
            std::cout<<" score cp "<<ColorScoreFactor[c]*int(100*float(extmv.value)/PawnValue);
    }

}



int main(int argc, char **argv){
	init_lut();
    init_magics();
	init_position(starting_fen);
	while (true) {
		std::string input;
		std::getline(std::cin, input);
		std::stringstream input_stream(input);
		std::string command;
		std::getline(input_stream, command, ' ');
		if (command == "uci"){
			std::cout << "id name "<<engineName<<" "<< VERSION << "\n";
			std::cout << "id author "<<author<<"\n";
			// std::cout << "option name\n";//hash: NO HASH TABLE\n";//<<type spin default " << (sizeof(StorageValue) << (HASH_KEY_LENGTH - 20)) << " min 1 max 65536\n"; //TODO
			std::cout << "uciok\n";
		}
		if (command == "debug") {
			std::string arg;
			std::getline(input_stream, arg, ' ');
			if (arg == "on")
				VERBOSE = 5;
			else if (arg == "off")
				VERBOSE = 1;
		}
		if (command == "isready")
			std::cout << "readyok\n";
		if (command == "ucinewgame"){
// 			ht_init(HASH_KEY_LENGTH); //TODO
		}
		if (command == "setoption"){
			std::string arg;
			std::getline(input_stream, arg, ' ');
			if (arg == "name") {
				std::getline(input_stream, arg, ' ');
				if (arg == "hashbits") {
					std::getline(input_stream, arg, ' ');
					if (arg == "value") {
						// input_stream >> HASH_KEY_LENGTH; //TODO
						// ht_init(HASH_KEY_LENGTH);  	    // TODO
						std::cout << "The 'hashbits' option is deprecated and will be removed in a future version" << std::endl;
						std::cout << "Use the 'hash' option instead to specify the hash table size in megabytes" << std::endl;
					}
				} else if (arg == "hash") {
					std::getline(input_stream, arg, ' ');
					if (arg == "value") {
						uint64_t hash_size_mb;
						// input_stream >> hash_size_mb; //TODO
						// int num_hash_entries = (hash_size_mb << 20) / sizeof(StorageValue);//TODO
						// HASH_KEY_LENGTH = 31 - __builtin_clz(num_hash_entries);//TODO
						// ht_init(HASH_KEY_LENGTH);//TODO
						std::cout << "Hash tables not implemented yet" << std::endl;
					}
				}
			}
		}
		if (command == "position"){
			std::string postype;
			std::getline(input_stream, postype, ' ');
			std::string fen;
			if (postype == "startpos"){
				fen = starting_fen;
			} else if (postype == "fen"){
				fen = "";
				for (int i = 0; i < 6; i++){
					std::string fen_chunk;
					std::getline(input_stream, fen_chunk, ' ');
					fen = fen + (i == 0 ? "" : " ") + fen_chunk;
				}
			}
			init_position(fen);
			// history = history.wipe();//TODO
			std::getline(input_stream, command, ' ');
		}
		if (command == "moves"){
			for (std::string move_text; std::getline(input_stream, move_text, ' ');){
				Move move = parse_move(move_text);
				// if (is_irreversible(board, move)){
				// 	history = history.wipe();
				// } else {
				// 	history = history.extend_root(board.EvalInfo.hash);
				// }
				// (wtm ? make_move<true> : make_move<false>) (board, move);
				// wtm = !wtm;
				MakeMove(move);
			}
		}		
		if (command == "show")
			RepresentBoard();
		if (command == "legal"){
			MoveList legal;
			legal = generate_legal(Position::sideToMove);
			PrintMoveList(legal);
		}
		if (command == "go"){
			uint64_t nodes = 0ULL;
			uint64_t move_counter = 0ULL;
			int depth = defaultDepth;
			int min_time_ms = INT_MAX;
			int max_time_ms = INT_MAX;
			for (std::string arg; input_stream >> arg;) {
				if (arg == "nodes") {
					input_stream >> nodes;
				} else if (arg == "depth") {
					input_stream >> depth;
				} else if (arg == "movetime") {
					int movetime;
					input_stream >> movetime;
					min_time_ms = movetime / 2;
					max_time_ms = movetime;
				} else if (arg == "wtime") {
					int wtime;
					input_stream >> wtime;
					if (Position::sideToMove == WHITE) {
						min_time_ms = wtime / 40;
						max_time_ms = wtime / 4;
					}
				} else if (arg == "btime") {
					int btime;
					input_stream >> btime;
					if (Position::sideToMove == BLACK) {
						min_time_ms = btime / 40;
						max_time_ms = btime / 4;
					}
				} else if (arg == "winc") {
					int winc;
					input_stream >> winc;
				} else if (arg == "binc") {
					int winc;
					input_stream >> winc;
				} else if (arg == "movestogo") {
					int movestogo;
					input_stream >> movestogo;
				} else {
					throw std::invalid_argument("Unsupported go option " + arg);
				}
			}
			// if ((nodes == INT_MAX) and (depth == INT_MAX) and (max_time_ms == INT_MAX)) {
			// 	throw std::invalid_argument("No limit on search");
			// } // TODO Implement time management
			ExtMove theBest;
			move_counter = 0;
			theBest = minmax(Position::sideToMove,-10000000, +10000000, depth,depth,move_counter, false);
			
			std::cout<<"info depth "<<depth;
			PrintExtMoveForUCI(theBest);
			std::cout<<" nodes "<<move_counter<<" pv "<<mvhuman(theBest.move) <<std::endl;
			std::cout << "bestmove "<<mvhuman(theBest.move)<<"\n";
			
		}
		if (command == "ponderhit"){
			throw std::invalid_argument("Pondering not supported");
		}
		if (command == "hashstats"){
			throw std::invalid_argument("Hash stats not supported");
			// ht_stats(); //TODO
		}
		if (command == "searchstats"){
			throw std::invalid_argument("Search stats not supported");
			// search_stats(); //TODO
		}
		// TODO searches the current position in the hashtable 
		if (command == "lookup"){
			throw std::invalid_argument("Lookup not supported");
			// const auto hash_key = wtm ? (wtm_hash ^ board.EvalInfo.hash) : board.EvalInfo.hash;
			// const auto hash_lookup_result = ht_lookup(hash_key);
			// if (hash_lookup_result.has_value()) {
			// 	std::cout << "Score: " << std::get<0>(hash_lookup_result.value()) << std::endl;
			// 	std::cout << "Move: " << format_move_xboard(std::get<1>(hash_lookup_result.value())) << std::endl;
			// 	std::cout << "Depth: " << (int) std::get<2>(hash_lookup_result.value()) << std::endl;
			// } else { std::cout << "Miss" << std::endl; }
		}
		if (command == "eval"){
			std::cout<<"Current position evaluated on material "<< EvalPosition()<<std::endl;
			// const auto mg_pst_eval = (board.White.King % 8 >= 4) ? 
			// 	((board.Black.King % 8 >= 4) ? board.EvalInfo.mg_kk : board.EvalInfo.mg_kq) :
			// 	((board.Black.King % 8 >= 4) ? board.EvalInfo.mg_qk : board.EvalInfo.mg_qq);
			// std::cout << "mg = " << mg_pst_eval << std::endl;
			// std::cout << "eg = " << board.EvalInfo.eg << std::endl;
			// std::cout << "phase_count = " << board.EvalInfo.phase_count << std::endl;
			// std::cout << "eval = " << (wtm ? eval<true>(board) : -eval<false>(board)) << std::endl;
		}
		if (command == "quit"){
			return 0;
		}			
	}
}





// # include <iostream>
// # include <sstream>
// # include <tuple>
// # include <climits>

// # include "history.hpp"
// # include "parse_format.hpp"
// # include "search.hpp"
// # include "movegen.hpp"
// # include "hashtable.hpp"
// # include "eval.hpp"
// # include "hashing.hpp"

// # ifndef RENGAR_VERSION
// # define RENGAR_VERSION unversioned
// # endif
// // https://stackoverflow.com/questions/2410976/how-can-i-define-a-string-literal-on-the-gcc-command-line
// # define STRINGIZE(x) #x
// # define STRINGIZE_VALUE_OF(x) STRINGIZE(x)
// const auto rengar_version = STRINGIZE_VALUE_OF(RENGAR_VERSION);

// void print_legal_moves(bool wtm, Board &board, ChecksAndPins cnp){
// 	if (wtm) {
// 		auto queue = generate_moves<true>(board, cnp, 0, 0, 0);
// 		while (!queue.empty()){
// 			std::cout << format_move_xboard(queue.top()) << "\n";
// 			queue.pop();
// 		}
// 	} else {
// 		auto queue = generate_moves<false>(board, cnp, 0, 0, 0);
// 		while (!queue.empty()){
// 			std::cout << format_move_xboard(queue.top()) << "\n";
// 			queue.pop();
// 		}
// 	}
// }


// void print_forcing_moves(bool wtm, Board &board, ChecksAndPins cnp){
// 	if (wtm) {
// 		auto queue = generate_forcing<true>(board, cnp);
// 		while (!queue.empty() and queue.top_prio() > qsearch_prio_cutoff){
// 			std::cout << format_move_xboard(queue.top()) << "\n";
// 			queue.pop();
// 		}
// 	} else {
// 		auto queue = generate_forcing<false>(board, cnp);
// 		while (!queue.empty() and queue.top_prio() > 0){
// 			std::cout << format_move_xboard(queue.top()) << "\n";
// 			queue.pop();
// 		}
// 	}
// }

// int HASH_KEY_LENGTH = 24;


// int main() {
// 	Board board;
// 	bool wtm = parse_fen(STARTING_FEN, board);
// 	History history;

// 	set_log_level(2);
// 	ht_init(HASH_KEY_LENGTH);

// 	while (true) {
// 		std::string input;
// 		std::getline(std::cin, input);
// 		std::stringstream input_stream(input);

// 		std::string command;
// 		std::getline(input_stream, command, ' ');

// 		if (command == "uci"){
// 			std::cout << "id name Rengar " << rengar_version << "\n";
// 			std::cout << "id author Thomas Swayze\n";
// 			std::cout << "option name hash type spin default " << (sizeof(StorageValue) << (HASH_KEY_LENGTH - 20)) 
// 				<< " min 1 max 65536\n";
// 			std::cout << "uciok\n";
// 		}
// 		if (command == "debug") {
// 			std::string arg;
// 			std::getline(input_stream, arg, ' ');
// 			if (arg == "on"){
// 				set_log_level(2);
// 			} else if (arg == "off"){
// 				set_log_level(1);
// 			}
// 		}
// 		if (command == "isready"){
// 			std::cout << "readyok\n";
// 		}
// 		if (command == "ucinewgame"){
// 			ht_init(HASH_KEY_LENGTH);
// 			initialize_move_order_arrays();
// 		}
// 		if (command == "setoption"){
// 			std::string arg;
// 			std::getline(input_stream, arg, ' ');
// 			if (arg == "name") {
// 				std::getline(input_stream, arg, ' ');
// 				if (arg == "hashbits") {
// 					std::getline(input_stream, arg, ' ');
// 					if (arg == "value") {
// 						input_stream >> HASH_KEY_LENGTH;
// 						ht_init(HASH_KEY_LENGTH);
// 						std::cout << "The 'hashbits' option is deprecated and will be removed in a future version" << std::endl;
// 						std::cout << "Use the 'hash' option instead to specify the hash table size in megabytes" << std::endl;
// 					}
// 				} else if (arg == "hash") {
// 					std::getline(input_stream, arg, ' ');
// 					if (arg == "value") {
// 						uint64_t hash_size_mb;
// 						input_stream >> hash_size_mb;
// 						int num_hash_entries = (hash_size_mb << 20) / sizeof(StorageValue);
// 						HASH_KEY_LENGTH = 31 - __builtin_clz(num_hash_entries);
// 						ht_init(HASH_KEY_LENGTH);
// 					}
// 				}
// 			}
// 		}
// 		if (command == "position"){
// 			std::string postype;
// 			std::getline(input_stream, postype, ' ');
// 			std::string fen;
// 			if (postype == "startpos"){
// 				fen = STARTING_FEN;
// 			} else if (postype == "fen"){
// 				fen = "";
// 				for (int i = 0; i < 6; i++){
// 					std::string fen_chunk;
// 					std::getline(input_stream, fen_chunk, ' ');
// 					fen = fen + (i == 0 ? "" : " ") + fen_chunk;
// 				}
// 			}
// 			wtm = parse_fen(fen, board);
// 			history = history.wipe();

// 			std::getline(input_stream, command, ' ');
// 		}
// 		if (command == "moves"){
// 			for (std::string move_text; std::getline(input_stream, move_text, ' ');){
// 				Move move = parse_move_xboard(move_text, board, wtm);
// 				if (is_irreversible(board, move)){
// 					history = history.wipe();
// 				} else {
// 					history = history.extend_root(board.EvalInfo.hash);
// 				}
// 				(wtm ? make_move<true> : make_move<false>) (board, move);
// 				wtm = !wtm;
// 			}
// 		}
// 		if (command == "show"){
// 			dump_board(board);
// 		}
// 		if (command == "legal"){
// 			auto cnp = wtm ? checks_and_pins<true>(board) : checks_and_pins<false>(board);
// 			print_legal_moves(wtm, board, cnp);
// 		}
// 		if (command == "forcing"){
// 			auto cnp = wtm ? checks_and_pins<true>(board) : checks_and_pins<false>(board);
// 			if (cnp.CheckMask != FULL_BOARD) { print_legal_moves(wtm, board, cnp); }
// 			else { print_forcing_moves(wtm, board, cnp); }
// 		}
// 		if (command == "go"){
// 			int nodes = INT_MAX;
// 			int depth = max_var_length;
// 			int min_time_ms = INT_MAX;
// 			int max_time_ms = INT_MAX;
// 			for (std::string arg; input_stream >> arg;) {
// 				if (arg == "nodes") {
// 					input_stream >> nodes;
// 				} else if (arg == "depth") {
// 					input_stream >> depth;
// 				} else if (arg == "movetime") {
// 					int movetime;
// 					input_stream >> movetime;
// 					min_time_ms = movetime / 2;
// 					max_time_ms = movetime;
// 				} else if (arg == "wtime") {
// 					int wtime;
// 					input_stream >> wtime;
// 					if (wtm) {
// 						min_time_ms = wtime / 40;
// 						max_time_ms = wtime / 4;
// 					}
// 				} else if (arg == "btime") {
// 					int btime;
// 					input_stream >> btime;
// 					if (not wtm) {
// 						min_time_ms = btime / 40;
// 						max_time_ms = btime / 4;
// 					}
// 				} else if (arg == "winc") {
// 					int winc;
// 					input_stream >> winc;
// 				} else if (arg == "binc") {
// 					int winc;
// 					input_stream >> winc;
// 				} else if (arg == "movestogo") {
// 					int movestogo;
// 					input_stream >> movestogo;
// 				} else {
// 					throw std::invalid_argument("Unsupported go option " + arg);
// 				}
// 			}
// 			if ((nodes == INT_MAX) and (depth == INT_MAX) and (max_time_ms == INT_MAX)) {
// 				throw std::invalid_argument("No limit on search");
// 			}
// 			Move move = (wtm ? search_for_move<true> : search_for_move<false>)(
// 				board, history, nodes, depth, min_time_ms, max_time_ms);
// 			std::cout << "bestmove " << format_move_xboard(move) << "\n";
// 		}
// 		if (command == "ponderhit"){
// 			throw std::invalid_argument("Pondering not supported");
// 		}
// 		if (command == "hashstats"){
// 			ht_stats();
// 		}
// 		if (command == "searchstats"){
// 			search_stats();
// 		}
// 		if (command == "lookup"){
// 			const auto hash_key = wtm ? (wtm_hash ^ board.EvalInfo.hash) : board.EvalInfo.hash;
// 			const auto hash_lookup_result = ht_lookup(hash_key);
// 			if (hash_lookup_result.has_value()) {
// 				std::cout << "Score: " << std::get<0>(hash_lookup_result.value()) << std::endl;
// 				std::cout << "Move: " << format_move_xboard(std::get<1>(hash_lookup_result.value())) << std::endl;
// 				std::cout << "Depth: " << (int) std::get<2>(hash_lookup_result.value()) << std::endl;
// 			} else { std::cout << "Miss" << std::endl; }
// 		}
// 		if (command == "eval"){
// 			const auto mg_pst_eval = (board.White.King % 8 >= 4) ? 
// 				((board.Black.King % 8 >= 4) ? board.EvalInfo.mg_kk : board.EvalInfo.mg_kq) :
// 				((board.Black.King % 8 >= 4) ? board.EvalInfo.mg_qk : board.EvalInfo.mg_qq);
// 			std::cout << "mg = " << mg_pst_eval << std::endl;
// 			std::cout << "eg = " << board.EvalInfo.eg << std::endl;
// 			std::cout << "phase_count = " << board.EvalInfo.phase_count << std::endl;
// 			std::cout << "eval = " << (wtm ? eval<true>(board) : -eval<false>(board)) << std::endl;
// 		}
// 		if (command == "quit"){
// 			return 0;
// 		}
// 		if (command == "moveorder"){
// 			show_move_order_values();
// 		}
// 	}

// 	return -1;
// }
