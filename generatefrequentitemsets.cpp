#include <bits/stdc++.h>
#define pb push_back
#define iOS ios::sync_with_stdio(false)
using namespace std;

#define THRESHOLD 5
#define NUM_TRANS 435   // number of transactions
#define NUM_CHILDREN 35 // number of max children
#define MAX_ATTR 35	    // number of attr + 1
#define minsup 0.4 //the minimum suppport

//the structure which stores the itemset along with its support
typedef struct itemset
{
    vector<int> values;
    double support;
}ITEMSET;

//structure to store the itemset in a hash tree
typedef struct HashTree
{
    int count = 0;
    int level = 0;
    HashTree* child[NUM_CHILDREN] = {NULL};
    vector<ITEMSET> items;
}HASHTREE;

vector<ITEMSET> F1;
vector<vector<ITEMSET> > frequent(MAX_ATTR);
vector<vector<ITEMSET> > candidate(MAX_ATTR);
vector<vector<int> > transactions;


bool comparekminus1(ITEMSET a, ITEMSET b)
{
	bool temp = true;
	for(int i=0; i<a.values.size()-1; i++)
	{
		if(a.values[i] != b.values[i])
		{
			temp = false;
			break;
		}
	}
	return temp;
}

bool ispresent(vector<ITEMSET> a, vector<int> val)
{
	bool res=false;
	for(int i = 0; i<a.size(); i++)
	{
		if(val == a[i].values)
		{
			res = true;
			break;
		}
	}
	return res;
}

bool checkfrequent(vector<int> a, int size)
{
	vector<int> temp;
	bool res = true;
	for(int k = 0;k < size-2; k++)
	{
		temp = a;
		temp.erase(temp.begin() + k);
		if(!(ispresent(frequent[size-1], temp)))
		{
			res = false;
			break;
		}
	}
	return res;
}

vector<ITEMSET> apriori_gen(vector<ITEMSET> freq)
{
	vector<ITEMSET> cand;
	if(freq[0].values.size() == 1)
	{
		for(int i=0; i < freq.size(); i++)
		{
			for(int j=i+1; j < freq.size(); j++)
			{
				ITEMSET temp;
				temp.values.pb(freq[i].values[0]);
				temp.values.pb(freq[j].values[0]);
				temp.support=0;
				cand.pb(temp);
			}
		}
		return cand;
	}
	
	for(int i = 0; i < freq.size(); i++)
	{
		for(int j = i + 1; j < freq.size(); j++)
		{
			if(comparekminus1(freq[i], freq[j]))
			{
				ITEMSET temp;
				temp.values = freq[i].values;
				temp.values.pb(freq[j].values[freq[j].values.size() - 1]);
				temp.support = 0;
				if(checkfrequent(temp.values, temp.values.size()))
					cand.pb(temp);
			}
		}
	}
	return cand;
}

HASHTREE* addValue(HASHTREE* node,ITEMSET it)
{

    if(node == NULL)
    {
        node = new HASHTREE;
        node -> count++;
        node -> items.pb(it);
        return node;
    }
    else
    {
        node -> items.pb(it);
        node -> count++;
        if((node -> count) > THRESHOLD && node -> level <= it.values.size() )
        {
            for(int j = 0;j < NUM_CHILDREN; j++)
            {
                node -> child[j] = new HASHTREE;
                (node -> child[j]) -> level = node -> level + 1;
            }
            for(int i = 0; i < node -> count; i++)
            {
                node -> child[(node -> items[i].values[node -> level])% NUM_CHILDREN] = addValue(node -> child[(node -> items[i].values[node -> level])% NUM_CHILDREN],node -> items[i]);
            }
        }
        return node;
    }
}

HASHTREE* createHashTree(vector<ITEMSET> values)
{
    HASHTREE* root=NULL;
    vector<ITEMSET> :: iterator i;
    for(i = values.begin(); i != values.end(); i++)
    {
        root = addValue(root, *i);
    }
    return root;

}
HASHTREE* updatesupport(HASHTREE* root, vector<int> trans,int size)
{
	if(root->count<=THRESHOLD || root->level==size)
	{
		for(int j=0;j<root->count;j++)
		{
			if(trans==root->items[j].values)
			{   
				root->items[j].support=root->items[j].support+1;
			}
		}	
	}
	else
	{
		root->child[trans[root->level] % NUM_CHILDREN] = updatesupport(root->child[trans[root->level] % NUM_CHILDREN],trans,size);
		
	}
	return root;
}

vector<ITEMSET> prunesupport(HASHTREE* root,int size)
{
	vector<ITEMSET> freq;
    if(root->count<=THRESHOLD || root->level==size)
    {
        for(int i=0;i<root->count;i++)
        {
        	double temp = (double) root->items[i].support;
        	
            if((((double)temp) / NUM_TRANS) >= minsup)
            {
            
            	freq.pb(root->items[i]);
            }
        }
    }
    else
    {
        for(int i=0;i<NUM_CHILDREN;i++)
        {
            vector<ITEMSET> temp = prunesupport(root->child[i],size);
            
            freq.insert(freq.end(),temp.begin(),temp.end());
            
        }
    }
    return freq;
}

void generate_subsets(vector<int> trans,int index,int size,bool val[],HASHTREE** proot)
{

	if(size==0)
	{
		vector<int> temp;
		for(int i=0;i<trans.size();i++)
			if(val[i]==true)	
			{
				temp.pb(trans[i]);
			}
	
		(*proot)=updatesupport(*proot,temp,temp.size()); 
		return;
	}
	else if(index==trans.size())	return ;
	else
	{
		val[index]=true;
		
		generate_subsets(trans,index+1,size-1,val,proot);
		val[index]=false;
		generate_subsets(trans,index+1,size,val,proot);
		return;
	}
	
	
}

vector<ITEMSET> generate_frequent(HASHTREE* root,int size)
{
	for(int i=0;i<transactions.size();i++)
	{
		bool val[transactions[i].size()]={false};
		generate_subsets(transactions[i],0,size,val,&root);
	}
	vector<ITEMSET> freq = prunesupport(root,size);
	return freq;
}

vector<ITEMSET> frequent_gen(vector<ITEMSET> cand, int size)
{
	HASHTREE* root = createHashTree(cand);
	vector<ITEMSET> freq = generate_frequent(root, size);
	return freq;
}


void apriori()
{
	// for F1 frequent set
	double F1candidate[MAX_ATTR] = {0};
	for(int i = 0; i < transactions.size(); i++)
	{
		for(int j = 0; j < transactions[i].size(); j++)
			F1candidate[transactions[i][j]]++;
	}

	// for support counting and pushing in F1
	for (int i = 1; i < MAX_ATTR; i++)
	{
		F1candidate[i] /= (double)NUM_TRANS;
		if (F1candidate[i] >= minsup)
		{
			itemset temp;
			temp.values.pb(i);
			temp.support = F1candidate[i] * (double)NUM_TRANS;
			frequent[1].pb(temp);
		}
	}	
	
	int k = 1;
	
	while(1)
	{
		
		k = k + 1;
		if (k == MAX_ATTR)	break;
		candidate[k] = apriori_gen(frequent[k - 1]);
		if (candidate[k].size() == 0)	
			break;
		frequent[k] = frequent_gen(candidate[k], k);
		if (frequent[k].size() == 0)	
			break;
	}
}


int main()
{
	iOS;
	string line;
	ifstream vote ("vote.arff");
	int count = 0;
	while(vote.is_open())
	{
		if( getline(vote, line) && count >= 213 && count < 648)
		{
			int len = line.length();
			string var = "";
			int attrVal = 1;
			
			for(int i = 0; i < len; i++)
			{
				if(line[i] != '\'' && line[i] != ',')
					var += line[i];					
			}
			vector<int> t;
			for(int i=0; i < 16; i++)
			{
				if(var[i] == 'n')
				{
					t.pb(attrVal);
					attrVal += 2;
				}
				else if(var[i] == 'y')
				{
					t.pb(attrVal + 1);
					attrVal += 2;
				}
				else if(var[i] == '?')
				{
					attrVal += 2;
				}

			}
			if(var[16] == 'r')  
				t.pb(attrVal);
			else
				t.pb(++attrVal);
			transactions.pb(t);
		}
		count++;
		if(count == 648)
			{
				vote.close();
				break;
			}
	}

	apriori();


	int numFreq = 0;
	for(int i=1;i<35;i++) 
	{
		numFreq += frequent[i].size();
	}
	
	
	
	fstream g("frequent_itemsets.txt", ios :: out);
	g << NUM_TRANS << " " << MAX_ATTR - 1<< endl;
	g << numFreq << endl;

	// generating the frequent itemsets for rule generation
	for(int i=1;i<35;i++) 
	{
		
		for(int j=0;j<frequent[i].size();j++)
		{
			g << i << " ";
			g << frequent[i][j].support << " ";
			for(int k=0;k<frequent[i][j].values.size();k++)
			{
				g << frequent[i][j].values[k] << " ";
			}
			g << endl;
			
		}
	
	}
	return 0;
}
