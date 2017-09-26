#include<bits/stdc++.h>
using namespace std;
#define CONFIDENCE 0.8

int num_trans;
int num_items;
int num_freq;
vector<vector<int> > frequent_itemsets;
map<vector<int>, int> support_count;
set<vector<vector<int> > > rules;
map<vector<vector<int> >, double> rule_confidence;
fstream f("frequent_itemsets.txt",ios::in);

void read_frequent_itemsets()
{
	f>>num_freq;
	for (int i = 0; i < num_freq; ++i)
	{
		int num_in_itemset,support_count_itemset;
		f>>num_in_itemset>>support_count_itemset;
		vector<int> temp(num_in_itemset);
		for(int j=0;j<num_in_itemset;j++)
			f>>temp[j];
		frequent_itemsets.push_back(temp);
		support_count[frequent_itemsets[i]] = support_count_itemset;
	}
}

void print(vector<int> a)
{
	for (int i = 0; i < a.size(); ++i)
	{
		cout<<a[i]<<" ";
	}
	cout<<endl;
}

void vector_intersection(vector<int> a, vector<int> b, vector<int> &res)
{
	int i=0,j=0;
	while(i<a.size() && j<b.size())
	{
		if(a[i]==b[j])
		{
			res.push_back(a[i]);
			i++;
			j++;
		}
		if(a[i]<b[j])
			i++;
		if(a[i]>b[j])
			j++;
	}
}

void vector_union(vector<int> a, vector<int> b, vector<int> &res)
{
	set<int> s(a.begin(),a.end());
	for (int i = 0; i < b.size(); ++i)
		s.insert(b[i]);
	for(set<int>::iterator it = s.begin(); it!=s.end(); ++it)
		res.push_back(*it);
}

void generate_rules(vector<int> itemset)
{
	if(itemset.size()<2)
		return;
	map<int,set<vector<vector<int> > > > temp_rules;
	//generate all one consequent rules
	set<vector<vector<int> > > one_cons;
	for (int i = 0; i < itemset.size(); ++i)
	{
		vector<int> consequent;
		consequent.push_back(itemset[i]);
		vector<int> antecedent;
		for(int j=0;j<itemset.size(); ++j)
		{
			if(j!=i)
			{
				antecedent.push_back(itemset[j]);
			}
		}
		int antecedent_support = support_count[antecedent];
		double temp_rule_conf = (double)support_count[itemset]/(double)antecedent_support;
		if(temp_rule_conf >= CONFIDENCE)
		{
			vector<vector<int> > temp;
			temp.push_back(antecedent);
			temp.push_back(consequent);
			one_cons.insert(temp);
			rules.insert(temp);
			rule_confidence[temp] = temp_rule_conf;
		}
	}
	temp_rules[1] = one_cons;
	for(int i=2;i<itemset.size() && temp_rules[i-1].size()!=0;i++)
	{
		set<vector<vector<int> > > prev_rules_set = temp_rules[i-1];
		set<vector<vector<int> > > plus_one_cons;
		vector<vector<vector<int> > > prev_rules(prev_rules_set.begin(),prev_rules_set.end());
		for(int j=0;j<prev_rules.size()-1;++j)
		{
			vector<vector<int> > r1 = prev_rules[j];
			vector<int> r1_antecedent = r1[0];
			vector<int> r1_consequent = r1[1];
			for(int k=j+1;k<prev_rules.size();++k)
			{
				vector<vector<int> > r2 = prev_rules[k];
				vector<int> r2_antecedent = r2[0];
				vector<int> r2_consequent = r2[1];
				vector<int> new_antecedent;
				vector<int> new_consequent;
				vector_intersection(r1_antecedent,r2_antecedent,new_antecedent);
				vector_union(r1_consequent,r2_consequent,new_consequent);
				if(new_antecedent.size()==0 || new_consequent.size()==0)
					continue;
				int antecedent_support = support_count[new_antecedent];
				double temp_rule_conf = (double)support_count[itemset]/(double)antecedent_support;
				if(temp_rule_conf >= CONFIDENCE)
				{
					vector<vector<int> > temp;
					temp.push_back(new_antecedent);
					temp.push_back(new_consequent);
					plus_one_cons.insert(temp);
					rules.insert(temp);
					rule_confidence[temp] = temp_rule_conf;
				}
			}
		}
		if(plus_one_cons.size()<2)
			break;
		temp_rules[i] = plus_one_cons;
	}
}

void write_rules()
{
	fstream h("valtoattr.txt",ios::in);
	int t;
	string temp;
	map<int,string> attrname;
	for (int i = 0; i < num_items; ++i)
	{
		h>>t>>temp;
		attrname[t] = temp;
	}
	fstream g("rules.txt",ios::out);
	vector<vector<vector<int> > > temp_rules(rules.begin(),rules.end());
	for (int i = 0; i < temp_rules.size(); ++i)
	{
		g<<"Confidence = "<<rule_confidence[temp_rules[i]]<<" Rule: ";
		for(int j=0;j<temp_rules[i].size();++j)
		{
			for(int k=0;k<temp_rules[i][j].size();k++)
			{
				g<<attrname[temp_rules[i][j][k]];
				if(k!=temp_rules[i][j].size()-1)
					g<<", ";
			}
			if(j==0)
				g<<" -> ";	
		}
		g<<endl;
	}
}

int main()
{
	f>>num_trans>>num_items;
	read_frequent_itemsets();
	for (int i = 0; i < num_freq; ++i)
	{
		generate_rules(frequent_itemsets[i]);
	}

	write_rules();
}