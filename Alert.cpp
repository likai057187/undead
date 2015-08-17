#include <iostream>
#include <sstream>
#include <string>

#include<vector>
#include<map>
#include<set>
#include<algorithm>
#include <numeric>

using namespace std;

/*
 * We include a parser below that you are free to use or replace.
 * It's mostly complete in functionality and can aid with development and
 * debugging.
 *
 * Please remember two points:
 *      1. The output it produces is meant only as a debugging aid when getting
 *      started.  Per the specification in the assignment, all the output below
 *      should be removed for the final specification (or not sent to stdout).
 *
 *      2. Remember we are NOT testing your abilities to parse text.  Using the
 *      below parser, as-is, is perfectly acceptable.
 */

enum ConsensusStat { MEAN, MEDIAN, MAX, MIN, STDDEV };

class Analyst {
	string m_name;
	double m_most_recent_estimate;
public:
	Analyst(){};
	Analyst(string s, double d):m_name(s),m_most_recent_estimate(d) {};
	~Analyst(){};

	double GetEstimate(void) {return m_most_recent_estimate; }
	void SetEstimate(double estimate) {m_most_recent_estimate = estimate; }

};

class Client {
	string m_name;
	bool m_sub_initiate;
	bool m_sub_outlier;
	double m_outlier;
	map<ConsensusStat,double> m_change_list;
	set<string> m_knowledge_of_analysts;
public:
	Client(){}
	Client(string s) {
		m_name = s;
		m_sub_initiate = false;
		m_sub_outlier = false;
		m_outlier = 0;
	}
	~Client() {};

	void SetSubInitiate(bool f) {m_sub_initiate = f; }
	bool GetSubInitiate(void) {return m_sub_initiate; }
	void SetSubOutlier(bool f) {m_sub_outlier = f; }
	bool GetSubOutlier(void) {return m_sub_outlier; }
	void SetOutlier(double val) {m_outlier = val; }
	double GetOutlier(void) {return m_outlier; }
	bool isNewAnalysts(string name) {return m_knowledge_of_analysts.find(name)==m_knowledge_of_analysts.end();}
	void AddKnowledgeOfAnalysts(string a) {m_knowledge_of_analysts.insert(a); }
	void updateChangeList(ConsensusStat cs, double pct) {m_change_list[cs] = pct; }
	map<ConsensusStat,double>& GetChangeList(void) {return m_change_list; }
};

class AlertSystem {
	map<string,Analyst> m_current_analysts;
	map<string,Client> m_current_clients;
	map<ConsensusStat,double> m_consensus;
	map<ConsensusStat,double> m_consensus_prev;
public:
	AlertSystem(){}
	~AlertSystem(){}
	
	void updateAnalysts(string name, double val);
	void updateClients(string name);
	void updateConsensus(void);

	//process initiate subscibe
	void processInitiate(string name);
	//process changes subscribe
	void processChanges(string name, ConsensusStat cs, double pct);
	//process outlier subscribe
	void processOutlier(string name, double val);
	//process unsubscribe
	void processUnsubscribe(string name);

	//check alerts
	void checkAlerts(string name);
};

void AlertSystem::updateAnalysts(string name, double val) {
	if(m_current_analysts.find(name)==m_current_analysts.end()) {
		Analyst newAnalyst(name,val);
		m_current_analysts[name] = newAnalyst;
	} else {
		m_current_analysts[name].SetEstimate(val);
	}
}

void AlertSystem::updateClients(string name) {
	if(m_current_clients.find(name)==m_current_clients.end()) {
		Client newClient(name);
		m_current_clients[name] = newClient;
	}
}

void AlertSystem::processInitiate(string name) {
	Client& c = m_current_clients[name];
	c.SetSubInitiate(true);
	map<string,Analyst>::iterator it;
	for(it=m_current_analysts.begin();it!=m_current_analysts.end();it++) {
		c.AddKnowledgeOfAnalysts(it->first);
	}
}

void AlertSystem::processChanges(string name, ConsensusStat cs, double pct) {
	Client& c = m_current_clients[name];
	c.updateChangeList(cs,pct);
}

void AlertSystem::processOutlier(string name, double val){
	Client& c = m_current_clients[name];
	c.SetSubOutlier(true);
	c.SetOutlier(val);
}

void AlertSystem::processUnsubscribe(string name) {
	m_current_clients.erase(name);
}

void AlertSystem::checkAlerts(string name) {
	map<string,Client>::iterator it;
	for(it=m_current_clients.begin();it!=m_current_clients.end();it++) {
		//check initiate
		if(it->second.GetSubInitiate() && it->second.isNewAnalysts(name)) {
			cout<<"alert "<<it->first<<": "<<name<<" "<<m_current_analysts[name].GetEstimate()<<" initiate"<<endl;
			it->second.AddKnowledgeOfAnalysts(name);
		}
		//check changes
		double pct;
		if(it->second.GetChangeList().find(ConsensusStat::MAX)!=it->second.GetChangeList().end() && m_consensus_prev.find(ConsensusStat::MAX)!=m_consensus_prev.end()) {
			pct = it->second.GetChangeList()[ConsensusStat::MAX];
			if(m_consensus_prev[ConsensusStat::MAX]*(pct/100)<=abs(m_consensus[ConsensusStat::MAX]-m_consensus_prev[ConsensusStat::MAX])) {
				cout<<"alert "<<it->first<<": "<<name<<" "<<m_current_analysts[name].GetEstimate()<<" change max "<<pct<<endl;
			}
		}

		if(it->second.GetChangeList().find(ConsensusStat::MIN)!=it->second.GetChangeList().end() && m_consensus_prev.find(ConsensusStat::MIN)!=m_consensus_prev.end()) {
			pct = it->second.GetChangeList()[ConsensusStat::MIN];
			if(m_consensus_prev[ConsensusStat::MIN]*(pct/100)<=abs(m_consensus[ConsensusStat::MIN]-m_consensus_prev[ConsensusStat::MIN])) {
				cout<<"alert "<<it->first<<": "<<name<<" "<<m_current_analysts[name].GetEstimate()<<" change min "<<pct<<endl;
			}
		}

		if(it->second.GetChangeList().find(ConsensusStat::MEAN)!=it->second.GetChangeList().end() && m_consensus_prev.find(ConsensusStat::MEAN)!=m_consensus_prev.end()) {
			pct = it->second.GetChangeList()[ConsensusStat::MEAN];
			if(m_consensus_prev[ConsensusStat::MEAN]*(pct/100)<=abs(m_consensus[ConsensusStat::MEAN]-m_consensus_prev[ConsensusStat::MEAN])) {
				cout<<"alert "<<it->first<<": "<<name<<" "<<m_current_analysts[name].GetEstimate()<<" change mean "<<pct<<endl;
			}
		}

		if(it->second.GetChangeList().find(ConsensusStat::MEDIAN)!=it->second.GetChangeList().end() && m_consensus_prev.find(ConsensusStat::MEDIAN)!=m_consensus_prev.end()) {
			pct = it->second.GetChangeList()[ConsensusStat::MEDIAN];
			if(m_consensus_prev[ConsensusStat::MEDIAN]*(pct/100)<=abs(m_consensus[ConsensusStat::MEDIAN]-m_consensus_prev[ConsensusStat::MEDIAN])) {
				cout<<"alert "<<it->first<<": "<<name<<" "<<m_current_analysts[name].GetEstimate()<<" change mean "<<pct<<endl;
			}
		}

		if(it->second.GetChangeList().find(ConsensusStat::STDDEV)!=it->second.GetChangeList().end() && m_consensus_prev.find(ConsensusStat::STDDEV)!=m_consensus_prev.end()) {
			pct = it->second.GetChangeList()[ConsensusStat::STDDEV];
			if(m_consensus_prev[ConsensusStat::STDDEV]*(pct/100)<=abs(m_consensus[ConsensusStat::STDDEV]-m_consensus_prev[ConsensusStat::STDDEV])) {
				cout<<"alert "<<it->first<<": "<<name<<" "<<m_current_analysts[name].GetEstimate()<<" change stddev "<<pct<<endl;
			}
		}

		//check outlier
		if(it->second.GetSubOutlier() && m_consensus_prev.find(ConsensusStat::MEAN)!=m_consensus_prev.end()) {
			double right_bound = m_consensus_prev[ConsensusStat::MEAN] + (it->second.GetOutlier()*m_consensus_prev[ConsensusStat::STDDEV]);
			double left_bound = m_consensus_prev[ConsensusStat::MEAN] - (it->second.GetOutlier()*m_consensus_prev[ConsensusStat::STDDEV]);
			if(m_current_analysts[name].GetEstimate()>=right_bound || m_current_analysts[name].GetEstimate()<=left_bound) {
				cout<<"alert "<<it->first<<": "<<name<<" "<<m_current_analysts[name].GetEstimate()<<" outlier "<<it->second.GetOutlier()<<endl;
			}
		}
	}
}

void AlertSystem::updateConsensus(void) {
	vector<double> mostRecents;
	map<string,Analyst>::iterator it;
	for(it=m_current_analysts.begin();it!=m_current_analysts.end();it++) {
		mostRecents.push_back(it->second.GetEstimate());
	}
	std::sort(mostRecents.begin(),mostRecents.end());

	//mean
	if(m_consensus.find(ConsensusStat::MEAN)!=m_consensus.end()) {
		m_consensus_prev[ConsensusStat::MEAN] = m_consensus[ConsensusStat::MEAN];
	}
	m_consensus[ConsensusStat::MEAN] = std::accumulate(mostRecents.begin(), mostRecents.end(), 0.0) / mostRecents.size();
	
	//median
	if(m_consensus.find(ConsensusStat::MEDIAN)!=m_consensus.end()) {
		m_consensus_prev[ConsensusStat::MEDIAN] = m_consensus[ConsensusStat::MEDIAN];
	}
	mostRecents.size()%2==0?m_consensus[ConsensusStat::MEDIAN]=(mostRecents[mostRecents.size()/2-1]+mostRecents[mostRecents.size()/2])/2:m_consensus[ConsensusStat::MEDIAN] = mostRecents[mostRecents.size() / 2];

	//maximum
	if(m_consensus.find(ConsensusStat::MAX)!=m_consensus.end()) {
		m_consensus_prev[ConsensusStat::MAX] = m_consensus[ConsensusStat::MAX];
	}
	m_consensus[ConsensusStat::MAX] = *std::max_element(mostRecents.begin(),mostRecents.end());

	//minimum
	if(m_consensus.find(ConsensusStat::MIN)!=m_consensus.end()) {
		m_consensus_prev[ConsensusStat::MIN] = m_consensus[ConsensusStat::MIN];
	}
	m_consensus[ConsensusStat::MIN] = *std::min_element(mostRecents.begin(),mostRecents.end());

	//standard-deviation
	double accum = 0.0;
	double m = m_consensus[ConsensusStat::MEAN];
	std::for_each (std::begin(mostRecents), std::end(mostRecents), [&](const double d) {
		accum += (d - m) * (d - m);
	});
	if(m_consensus.find(ConsensusStat::STDDEV)!=m_consensus.end()) {
		m_consensus_prev[ConsensusStat::STDDEV] = m_consensus[ConsensusStat::STDDEV];
	}
	if(mostRecents.size()>1) {
		m_consensus[ConsensusStat::STDDEV] = sqrt(accum / (mostRecents.size()-1));
	}
}

bool
parsePoint(stringstream &ss, string &analyst, double &value)
{
    try { ss >> analyst; } catch (...) { return false; }
    if (ss.eof()) return false;
    try { ss >> value; }   catch (...) { return false; }
    return true;
}

bool
parseSubscribeChange(stringstream &ss, ConsensusStat &stat, int &pct)
{
    string statInput;
    try { ss >> statInput; } catch (...) { return false; }
    if (ss.eof()) return false;
    try { ss >> pct; }       catch (...) { return false; }

    if (0 == statInput.compare("mean"))
        stat = MEAN;
    else if(0 == statInput.compare("median"))
        stat = MEDIAN;
    else if(0 == statInput.compare("max"))
        stat = MAX;
    else if(0 == statInput.compare("min"))
        stat = MIN;
    else if(0 == statInput.compare("stddev"))
        stat = STDDEV;
    else
        return false;

    return true;
}

bool
parseSubscribeOutlier(std::stringstream &ss, int &n)
{
    try { ss >> n; }      catch (...) { return false; }
    return true;
}

bool
parseUnsubscribe(stringstream &ss, string &client)
{
    try { ss >> client; } catch (...) { return false; }
    return true;
}

bool
parseLine(string &line, AlertSystem& as)
{
    stringstream ss(line);
    string cmd, subcmd, client, analyst;
    ConsensusStat stat;
    double estimate;
    int n, pct;

    try { ss >> cmd; } catch (...) { return false; }

    // point <analyst> <estimate>
    if ("point" == cmd
            && !ss.eof()
            && parsePoint(ss, analyst, estimate)
            && ss.eof())
    {
        //cout << "Recieved a point from analyst " << analyst << " for "
        //     << estimate << endl;

		//update analysts list and their value
		as.updateAnalysts(analyst,estimate);

		//update the consensus
		as.updateConsensus();

		//check alerts
		as.checkAlerts(analyst);

        return true;
    }

    // subscribe <client> <cmd...>
    else if ("subscribe" == cmd)
    {
        try { ss >> client >> subcmd; } catch (...) { return false; }

		//update client list
		as.updateClients(client);

        // subscribe <client> initiate
        if ("initiate" == subcmd
                && ss.eof())
        {
            //cout << "Received initiate subscription from client " << client
            //     << endl;

			//process initiate
			as.processInitiate(client);
			

            return true;
        }

        // subscribe <client> change <stat> <iPct>
        else if ("change" == subcmd
                && !ss.eof()
                && parseSubscribeChange(ss, stat, pct)
                && ss.eof())
        {
            //cout << "Received consensus change subscription from client "
            //     << client << " for stat " << stat << " exceeding "
            //     << pct << " percent" << endl;
            
			//process changes
			as.processChanges(client,stat,pct);

			return true;
        }

        // subscribe <client> outlier <num>
        else if ("outlier" == subcmd
                && !ss.eof()
                && parseSubscribeOutlier(ss, n)
                && ss.eof())
        {
            //cout << "Received outlier subscription from client " << client
            //     << " for " << n << " standard deviations" << endl;

			//process outlier
			as.processOutlier(client,n);

            return true;
        }
        
       
    }
    // unsubscribe <client>
    else if (0 == cmd.compare("unsubscribe")
            && !ss.eof()
            && parseUnsubscribe(ss, client)
            && ss.eof())
    {
        //cout << "Parsed an unsubscribe command for client " << client
        //     << endl;

		//process unsubscribe
		as.processUnsubscribe(client);

        return true;
    }

    return false;
}

/*
 * Simple test driver for the parser.
 */

int
main()
{
    string line;
	AlertSystem as;

    while (getline(cin, line, '\n'))
    {
        if (!parseLine(line,as))
            cout << "Failed to parse line!" << endl;
    }

    return 0;
}