#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>

using namespace std;
using namespace std::chrono;

int number_of_voters = 5;
string s = "VID";
mutex mtx;
std::vector<char> del;
std::vector<string> delTO;

class Ballot 
{
	public:
			struct Voter
			{
				int weight;
				bool voted;
				string delegate;
				int vote;
			};

			struct Proposal 
			{
				string name;
				int voteCount;
			};

			string chairperson;
			map<string, Voter> voters;
			vector<Proposal> proposals;

	
			Ballot(string msg_sender, vector<string> proposalNames) 
			{
				chairperson = msg_sender;
				voters[chairperson].weight = 1;

				for (int i = 0; i < proposalNames.size(); i++) 
				{
					proposals.push_back(Proposal({name : proposalNames[i], voteCount : 0}));
				}
			}

			void giveRightToVote(string msg_sender, string voter) 
			{
				if (msg_sender != chairperson || voters[voter].voted)
					return;
				voters[voter].weight = 1;
			}

		  /*void show_map()
		  {
		    map<string,Voter>::iterator itr;
		    for(itr=voters.begin();itr!=voters.end();itr++)
		    {
		      cout<<itr->first<<"\n"<<itr->second.vote<<itr->second.delegate<<"\n\n";
		    }
		  }*/

			void delegate(string msg_sender, string to) 
			{
				
				if (voters[msg_sender].voted) 
				{
					cout << "\nYou have already voted";
					return;
				}

				if (to == msg_sender) 
				{
					cout << "\nCannot delegate yourself";
					return;
				}

				while (voters[to].delegate != "ID0" && voters[to].delegate != msg_sender && voters[to].delegate!="") 
				{
					to = voters[to].delegate;
				}

				voters[msg_sender].voted = true;
				voters[msg_sender].delegate = to;
				
		        unique_lock<mutex> lock(mtx);
				if (voters[to].voted) 
				{
					proposals[voters[to].vote].voteCount += voters[msg_sender].weight;
				} else 
				{
					voters[to].weight += voters[msg_sender].weight;
				}
		        lock.unlock();
			}

			void vote(string msg_sender, int proposal) 
			{
				
				if (voters[msg_sender].voted)
					return;
				voters[msg_sender].voted = true;
				voters[msg_sender].vote = proposal;
		               
		        unique_lock<mutex> lock(mtx);
				proposals[proposal].voteCount +=voters[msg_sender].weight;
		        lock.unlock();
			}

			int winningproposal() 
			{
				int winningProposal, winningVoteCount = 0;
				for (int p = 0; p < proposals.size(); p++)
				 {
					if (proposals[p].voteCount > winningVoteCount) 
					{
						winningVoteCount = proposals[p].voteCount;
						winningProposal = p;
					}
				}
		        cout<<winningVoteCount<<"\n";
				return winningProposal;
			}    
};

void delegating1(Ballot &b)
{
	string s3;
	for (int i = 0; i < (number_of_voters/2); i++)
	{
		s3 = s + to_string(i);
			
		if (del[i]== 'y')
	    {
			b.delegate(s3, delTO[i]);
		}
	    else
	    {		          
	        b.vote(s3,stoi(delTO[i]));    
	    }
	}
    
}

void delegating2(Ballot &b)
{
	string s3;
	for (int i = (number_of_voters/2); i < number_of_voters; i++)
	{
		s3 = s + to_string(i);
		
		if (del[i]== 'y')
	    {
			b.delegate(s3, delTO[i]);
		}
	    else
	    {
	      
	        b.vote(s3,stoi(delTO[i]));    
	    }
	}
}

int main() 
{
	auto start = chrono::steady_clock::now();
		        
	vector<string> pro_name;
	pro_name.push_back("abc0");
	pro_name.push_back("abc1");
	pro_name.push_back("abc2");
	pro_name.push_back("abc3");
	Ballot b("ID0", pro_name);
	int j;
	string s1,s2,s3,s4,s5;
        

  	for (int i = 0; i < number_of_voters; i++) 
  	{
		s2 = s + to_string(i);
		b.giveRightToVote("ID0", s2);
	}

	del.push_back('y');
	del.push_back('y');
	del.push_back('n');
	del.push_back('y');
	del.push_back('y');
	delTO.push_back("VID3");
	delTO.push_back("VID4");
	delTO.push_back("0");
	delTO.push_back("VID1");
	delTO.push_back("VID2");
        
    thread t1(delegating1, ref(b));
    thread t2(delegating2, ref(b));

    t1.join();
    t2.join();

    cout << "\nWinning proposal number is = " << b.winningproposal() << "\n";

    
    auto end = chrono::steady_clock::now();

    
    auto diff = end - start;
   cout << chrono::duration <double, milli> (diff).count() << " ms" << endl;

	return 0;
}

