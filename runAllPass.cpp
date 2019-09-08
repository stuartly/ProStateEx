#include<iostream>
#include<fstream>
#include<map>
#include<string>
#include<cstring>
#include<stdio.h>
using namespace std;

#define OPT_COMMAND "./llvm/build/bin/opt"
#define OUTPUT_PARAM "/dev/null"


class Config 
{
public:
    Config(string filename);
	void runAll(string ir_filepath);
private:
	map<string,string> CFGTest;

};

int main(int argc, const char *argv[])
{
///	cout<<"the number of argc is "<<argc<<endl;
  //  cout<<"the first argv is "<<argv[0]<<endl;
  //  cout<<"the second argv is "<<argv[1]<<endl;
 	if(argc!=3)
	{
		cout<<"please input the paths of configure file and target IR file\n";
	}
	Config configure(argv[1]);
//	system("echo hello");
	configure.runAll(argv[2]);
    return 0;
}

Config::Config(string filename)
{
    ifstream configFile;
    string path = "../conf/setting.conf";
    configFile.open(filename.c_str());
    string str_line;
    if (configFile.is_open())
    {
        while (!configFile.eof())
        {
            getline(configFile, str_line);
            if ( str_line.find('#') == 0 )
            {
                continue;
            }    
			else if (str_line.find("CFGTest")!=str_line.npos)
			{
				getline(configFile,str_line);
				str_line.erase(0,str_line.find_first_not_of(" "));
				size_t pos = str_line.find('=');
            	string str_key = str_line.substr(0, pos);
            	string str_value = str_line.substr(pos + 1);
            	CFGTest.insert(pair<string, string>(str_key, str_value));
				//cout<<"the first pair is *"<<str_key<<"* and *"<<str_value<<"*"<<endl;
				string sample="true";
				if(strcmp(str_value.c_str(),sample.c_str())==0)
				{
					getline(configFile,str_line);
					for(int i=0;(!configFile.eof())&&(str_line.find("list")!=str_line.npos);i++)
					{
						str_line.erase(0,str_line.find_first_not_of(" "));
						size_t pos = str_line.find('=');
						string str_key = str_line.substr(0, pos);
            			string str_value = str_line.substr(pos + 1);
            			CFGTest.insert(pair<string, string>(str_key, str_value));
						//cout<<"the next pair is *"<<str_key<<"* and *"<<str_value<<"*"<<endl;
						getline(configFile,str_line);
					}

				}
//				else
//					cout<<"the str_value is ******"<<str_value<<"******"<<endl;
			}
//			else
//				cout<<"the str_line is "<<str_line<<endl;
        }
    }
    else
    {    
        cout << "Cannot open the configure file\n";
        exit(-1);
    }

    return;
}

void Config::runAll(string ir_filepath)
{
	string str_enable="enable";
	string str_true="true";

	/* cfgtest */
	#define cfgtest_path "./llvm/build/lib/LLVMCFGTest.so"
	if(strcmp(CFGTest[str_enable].c_str(),str_true.c_str())==0)
	{
		string command=OPT_COMMAND;
		command=command+" -load "+cfgtest_path+" -cfg-funcs ";
	//	for(map<string,string>::iterator iter;)
		string str_list="list";
		string str_number;
		for(int i=1;;i++)
		{
			//itoa(i,str_number,10);
			//char str[20];
			//sprintf(&str,"%d",i);
			str_number=to_string(i);
			string tmpstr=str_list+str_number;
			map<string,string>::iterator iter=CFGTest.find(tmpstr);
			if(iter!=CFGTest.end())
			{
		//		command=command+"-"+tmpstr+" "+iter->second+" ";
				command=command+"-lists "+iter->second+" ";
			}
			else
			{
				//cout<<"the key is "<<tmpstr<<endl;
				break;
			}
			//cout<<command<<endl;
		}

		command=command+"< "+ir_filepath+" > "+OUTPUT_PARAM;
	//	cout<<command<<endl;
		system(command.c_str());
	}
}
