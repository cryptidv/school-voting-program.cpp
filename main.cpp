// School council elections made in C++
// For computers science pre-release
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <chrono>
#include <thread>
#include <sstream>  


// Util functions
// Basic sleep method (should work on any operating system https://stackoverflow.com/a/1658429)
void sleep(int seconds) {
    // suspend the thread
    std::this_thread::sleep_for(std::chrono::milliseconds(seconds * 1000));
}

void run(std::vector<std::string> candidates, std::map<std::string, int> * candidates_votes, int student_count, int * abstentions, int * total_votes);
void ask_select_vote(std::map<std::string, int> * candidates_votes, int * abstentions, int * total_votes);

int main() {
    
    // Consts
    const int MAX_STUDENTS = 50;
    const int MIN_STUDENTS = 5;
    const int MAX_CANDIDATES = 4;

    // Var decleration
    std::vector<std::string> candidates;
    std::map<std::string, int> candidates_votes;

    int abstentions = 0;
    int total_votes = 0;

    std::string tutor_group_name;
    
    int student_count;
    int candidate_count;

    // Setup prompts
    std::cout << "Please enter the name of the tutor group (7A, 7B, 8A, 8B, etc..): ";
    std::cin >> tutor_group_name;
    
    std::cout << "Please enter the number of students in this tutor group: ";
    std::cin >> student_count;
    
    if (student_count > MAX_STUDENTS || student_count < MIN_STUDENTS) {
        std::cout << "The Maximum number of students allowed is \033[1;36m" << MAX_STUDENTS << "\033[0m and the minimum is \033[1;36m" << MIN_STUDENTS << "\033[0m.\n";
        return 1;
    }

    std::cout << "Please enter the number of candidates in this tutor group (max \033[1;36m" << MAX_CANDIDATES << "\033[0m): ";
    std::cin >> candidate_count;

    if (candidate_count > MAX_CANDIDATES || candidate_count < 2) {
        std::cout << "The maximum number of candidates allowed is \033[1;36m" << MAX_CANDIDATES << "\033[0m and the minimum is \033[1;36m2\033[0m.\n";
        return 1;
    }

    for (int i{}; i < candidate_count; i++) {
        std::string name;
        std::cout << "Please enter the name of candidate \033[0;106m\033[1;30m#" << i+1 << "\033[0m: \033[1;93m";
        std::cin >> name;
        std::cout << "\033[0m"; // ends the input color
        candidates.push_back(name);
        candidates_votes.insert(std::pair<std::string, int>(name, 0));
    }

    std::cout << "\n\nThe following will be for the students alone\n================================\n";
    // run the voting channel now nothing should return to the main function
    run(candidates, &candidates_votes, student_count, &total_votes, &abstentions);
    
    std::cout << "\n\033[0;90mProgram finished...\033[0m\n";
    return 0;
}


/**
 * program::std::run()
 * 
 * Runs the primary voting function
 * Asks each student for their vote
 * */
void run(std::vector<std::string> candidates, std::map<std::string, int> * candidates_votes, int student_count, int * abstentions, int * total_votes) { 

    std::vector<std::string> voter_ids;

    // Have each student vote
    for (int i{}; i < student_count;) {
        std::cout << "This is student \033[1;93m#" << i+1 << "\033[0m\n";

        // Ask the student for their voter id and check it
        std::string v_id;
        std::cout << "Please enter your voter id \033[1;94m(Case Sensitive)\033[0m: \033[1;96m";
        std::cin >> v_id;
        std::cout << "\033[0m"; // End the input color

        if (std::find(voter_ids.begin(), voter_ids.end(), v_id) == voter_ids.end()) {
            // Not found
            ask_select_vote(candidates_votes, abstentions, total_votes);
            voter_ids.push_back(v_id);

            if (i < student_count - 1) {
                std::cout << "\n\033[1;93mNow preparing for next student\033[0m\n"; 
            } else {
                std::cout << "\n\033[1;96m======================================================\033[0m\n\033[1;92mAll students have now voted, please wait for results..\033[0m\n\033[1;96m======================================================\033[0m\n"; 
            }

            sleep(3);
            std::cout << "\n";
            i++;
        } else {
            // found
            std::cout << "You have already voted with id \033[1;94m" << v_id << "\033[0m. You can only vote ONCE.\n";
            sleep(2);
            std::cout << "\n";
            continue;
        }
    }

    // Pick winner or see if tie
    int winner_score = 0;
    std::string winner;

    // Tie variables
    bool is_tie = false;
    std::vector<std::string> ties;

    for (auto const& [name, score] : *candidates_votes) {
        if (score > winner_score) {
            winner_score = score;
            winner = name;
        } else if (score == winner_score) {
            is_tie = true;
            ties.push_back(winner);
            ties.push_back(name);
        }
    }

    if (is_tie) {
        int tie_count = ties.size();
        std::cout << "There has been a tie between \033[1;96m" << tie_count << "\033[0m candidates:\n";
        candidates_votes->clear(); // Clears the candidates votes to apply new candidates.
        for (std::string c : ties) {
            std::cout << "\u2022 \033[1;94m" << c << "\033[0m\n";
            candidates_votes->insert(std::pair<std::string, int>(c, 0));
        } 
        std::cout << "The voting will be repeated in 10 seconds with these " << tie_count << " candidates. Please get ready.. All students are required to vote (even if you abstained in the previous session)\n";
        std::cout << "\n";
        sleep(10);
        run(ties, candidates_votes, student_count, abstentions, total_votes);
    } else {
        std::cout << "\033[1;92mThe WINNER is " << winner << "!\033[0m\033[1;96m Votes: " << winner_score << "\033[0m\n";
        std::cout << "======= Results =======\n";
        // Congrats! announce winner with scores etc..
        for (auto const& [name, score] : *candidates_votes) {
            int perc = score * ( *total_votes / 100) * 100;
            std::cout << "\033[1;96m" << name << "\t\033[1;92m" << score << " votes \033[1;93m(" << perc << "% of all voters)\033[0m\n";
        }
        std::cout << "=======================\n";
        std::cout << "Total votes: " << *total_votes << "\nTotal abstentions: " << *abstentions << " (" << *abstentions * (student_count / 100) * 100 << ")\n";
    }
}


/**
 * program::std::ask_select_vote()
 * 
 * Asks and saves the votes 
 * */
void ask_select_vote(std::map<std::string, int> * candidates_votes, int * abstentions, int * total_votes) {

    std::string choice;

    // Create the string menu thingy for the students to select a candidate.
    std::map<int, std::string> maps_candidates;
    std::string list_str;
    int u = 1;
    for (auto const& [c_name, val] : *candidates_votes) {

        maps_candidates.insert(std::pair<int, std::string>(u, c_name));

        std::stringstream ss;
        std::string str;
        ss << u;
        ss >> str;
        list_str += "[\033[1;93m" + str + "\033[0m]: \033[1;94m" + c_name + "\033[0m\n";
        
        // Increment u
        u++;
    }



    std::cout << "Here is a list of possible candidates to vote on:\n";
    std::cout << list_str;
    std::cout << "To select a candidate, input the corresponding number.\n";

    while (1) {
        std::cout << "Please enter a number or \033[1;94m'a'\033[0m to abstain: \033[1;93m";
        std::cin >> choice;
        std::cout << "\033[0m";


        

        if (choice == "a") {
            *abstentions++;
            std::cout << "Your vote has been succesfully registered. Remember you can only vote ONCE..\n";
            break;
        } else {
            
            // Check for invalid options.
            try {
                if (std::stoi(choice) > u) {
                    std::cout << "'\033[1;92m" + choice + "\033[0m' is not a valid option... Please try again..\n";
                    continue;
                }
            } catch (...) {
                std::cout << "'\033[1;91m" + choice + "\033[0m' is not a valid option... Please try again..\n";
                sleep(1);
                continue;
            }

            std::string c_name = maps_candidates.at(std::stoi(choice));
            // Increment vote in map
            candidates_votes->at(c_name) += 1;
            *total_votes++;
            std::cout << "Your vote has been succesfully registered. Remember you can only vote ONCE..\n";
            break;
        }
    }
    
}
