#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <random>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

using namespace std;
namespace fs = std::filesystem;

const int IMGDIM=50;
const int WIDTH=IMGDIM*5;
const int HEIGHT=IMGDIM*2;
bool sdlLoaded=true;

SDL_Window* window=NULL; //the window we're rendering to
SDL_Surface* windowSurface=NULL; //the window's surface
int totalpix;

SDL_Surface* loadSurface( std::string path , SDL_Surface* gScreenSurface)
{
	//The final optimized image
	SDL_Surface* optimizedSurface = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
	}
		else
	{
		//Convert surface to screen format
		optimizedSurface = SDL_ConvertSurface( loadedSurface, gScreenSurface->format, 0 );
		if( optimizedSurface == NULL )
		{
			printf( "Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	return optimizedSurface;
};

std::size_t countpictures()
{
    std::size_t number_of_files = 0u;
    for (auto const & file : std::filesystem::directory_iterator("res/pictures"))
    {
        ++number_of_files;
    }
    return number_of_files;
};

bool isSdigit(std::string inputstring, int startP, int endP)//ignores spaces
{
    int i;
    //printf("lenght:%i\n",inputstring.length());
    if((endP>inputstring.length())||(startP>inputstring.length())||(endP<startP)){/*printf("wrong start & endpoints, false\n");*/return false;};

    for(i=startP; i<=endP;i++)
    {
        //printf("%i: %c\n", i, inputstring[i]);
        if(i>inputstring.length()){/*printf("%i>lenght during run, false\n",i);*/return true;};
        if((isdigit(inputstring[i])==0)&&(inputstring[i]!=' ')){/*printf("@%i: %c return false\n",i,inputstring[i]);*/return false;};
    };
    //printf("notchar notfound, return true\n");
    return true;

}

void clearscreen(){printf("\033c" );};
std::string fileselect(std::string path)
{
    int arg1=1;
    int arg2=0;
    std::string tempstring;
            for (const auto & entry : fs::directory_iterator(path.c_str()))
            {
                //printf("arg1 %i path %s \n",arg1,entry.path().c_str());
                tempstring.clear();
                //printf("tempstring %s\n",tempstring.c_str());
                tempstring.assign(entry.path().c_str());
                //printf("tempstring %s\n",tempstring.c_str());
                printf("%i) %s \n",arg1,tempstring.c_str());
                arg1++;
                arg2++;
            };
            printf("\n>");
            //arg2 now contains the total number of files;

    arg1=0;
    int arg3=0;

            do
            {
                tempstring.clear();
                getline(cin,tempstring);
                arg1=atoi(tempstring.c_str());
                if((arg1<0)||(arg1>arg2))
                {
                    printf("%i: choice out of bounds\n",arg1);
                    printf("\n>");
                }
                else{arg3=1;};
            }while(arg3==0);
            if(arg1==0){printf("returning...\n");return "NOFILE";};

            arg2=0;
            //arg1 now contains a bounded number, to match with the iterator arg2;

            tempstring.clear();

            for (const auto & entry : fs::directory_iterator(path.c_str()))
            {
                arg2++;
                if(arg1==arg2)
                {
                    tempstring.assign(entry.path());
                    //printf("got filename %s\n",tempstring.c_str());
                };

            };
            return tempstring;

};

int NextArgStart(std::string mystring, int startpos) //find the pos of next nonblank, nonend character; presupposes the string doesn't start with blanks
{
    //printf("NAS of '%s'\n",mystring.c_str());
    while((mystring[startpos]!=' ')&&(mystring[startpos]!='\0'))
    {
        //printf("NAS endsearch: nonblank nonend found @ %i(%c)\n",startpos,mystring[startpos]);
        startpos++;
    };
    //printf("NAS: endarg position %i(%c)\n",startpos,mystring[startpos]);
    if(mystring[startpos]=='\0'){/*printf("NAS: current argument reaches the end. no further arguments\n");*/ return -1;};

    while((mystring[startpos]==' ')&&(mystring[startpos]!='\0'))
    {
        //printf("NAS startsearch: blank nonend found @ %i(%c)\n",startpos,mystring[startpos]);
        startpos++;
    };
    //printf("NAS: startarg position %i(%c)\n",startpos,mystring[startpos]);
    if(mystring[startpos]=='\0'){/*printf("NAS: afterargument blanks reach the end. no further arguments\n");*/ return -1;}
    else{return startpos;};
};

std::string trimstring (std::string mystring) //trims blanks before and after
{
    while(mystring[0]==' ')
    {
        mystring.erase(0,1);
    };
    while(mystring.back()==' ')
    {
        mystring.pop_back();
    };
    return mystring;
};

typedef struct arglist{std::string command; int arg1; int arg2; int arg3; int arg4; std::string args;}arglist;

arglist getargs(std::string input)
{
    std::string command;
    std::string tempstring;
    int arg1,arg2,arg3,arg4;
    std::string args;
    arg1=arg2=arg3=arg4=0;
    command.clear();
    args.clear();

    //trim input
    input=trimstring(input);
    //printf("trimmed input '%s'\n",input.c_str());
    if(input.length()==0){/*printf("no command issued->repeat previous\n");command.assign("cm_previous");*/}
    else
    {
        //find arg1
        arg1=NextArgStart(input,0);
        if(arg1==-1)
        {
            //printf("no arg1\n");
            command.append(input);
            command=trimstring(command);
            //printf("got command '%s'\n",command.c_str());
            arg1=arg2=arg3=arg4=0;
        }
        else
        {
            //printf("arg1 @%i(%c)\n",arg1,input[arg1]);
            command.append(input.substr(0,arg1));
            command=trimstring(command);
            //printf("got command '%s'\n",command.c_str());

            arg2=NextArgStart(input,arg1);
            if(arg2>0){arg3=NextArgStart(input,arg2);/*printf("arg2 @%i(%c)\n",arg2,input[arg2]);*/};
            if(arg3>0){arg4=NextArgStart(input,arg3);/*printf("arg3 @%i(%c)\n",arg3,input[arg3]);*/};
            if(arg4>0){/*printf("arg4 @%i(%c)\n",arg4,input[arg4]);*/};
        };
    };

    //now to get the arguments
    if((arg1>0))//if it wasn't found it's negative; procedures have an exception for that; -6 is the mask for custom roller index->name
    {
        if /*((input[arg1]=='d')&&(isdigit(input[arg1+1])==true))*/ (isdigit(input[arg1])==false)
        {
            //NOTE: A negative number will be detected as a name
            //printf("dx-format or diename-format argument 1 detected\n");

            if(arg2>0)
            {args.assign(input.substr(arg1,arg2-arg1));}
            else{args.assign(input.substr(arg1));};

            if ((input[arg1]=='d')&&(isSdigit(input,arg1+1,arg2)==true)){/*printf("dx format detected->arg1=0\n");*/arg1=0;}else{/*printf("diename format detected-->arg1=-6");*/arg1=-6;};

            //printf("args:'%s' arg1:%i\n",args.c_str(),arg1);
        }
        else
        {
            //printf("integer arg1 detected\n");
            if(arg2>0)
            {arg1=atoi(input.substr(arg1,arg2-arg1).c_str());}
            else{arg1=atoi(input.substr(arg1).c_str());};
            arg1=abs(arg1); //arg 1 points to something
            //printf("arg1:%i\n",arg1);
        };
    };

    if(arg2>0) //if it wasn't found it's negative. procedures have an exception for that
    {
        if(arg3>0)
        {arg2=atoi(input.substr(arg2,arg3-arg2).c_str());}
        else{arg2=atoi(input.substr(arg2).c_str());};
        arg2=abs(arg2); //arg 2 is a position or a number of times
        //printf("arg2:%i\n",arg2);
    };

    if(arg3>0) //only used for bonuses; if it wasn't found, put it to 0
    {
        if(arg4>0)
        {arg3=atoi(input.substr(arg3,arg4-arg3).c_str());}
        else{arg3=atoi(input.substr(arg3).c_str());};
        //printf("arg3:%i\n",arg3);
    }else{arg3=0;};

    if(arg4>0) //only used for boons/banes; if it wasn't found, put it to 0
    {
        arg4=atoi(input.substr(arg4).c_str());
        //printf("arg4:%i\n",arg4);
    }else{arg4=0;};

    args=trimstring(args);

    //printf(">>>GETARGS: returning: '%s' '%i' '%i' '%i' '%i' '%s' \n",command.c_str(),arg1,arg2,arg3,arg4,args.c_str());

    return {command,arg1,arg2,arg3,arg4,args};
};


std::string uR (std::string input) //replaces underscores with spaces
{
    std::string output;
    output.assign(input);
    std::replace(output.begin(), output.end(), '_', ' ');
    //printf("underscore replacer: returning '%s'\n",output.c_str());
    return output;
};

std::string sR (std::string input) //replaces spaces with underscores
{
    std::string output;
    output.assign(input);
    std::replace(output.begin(), output.end(), ' ', '_');
    //printf("underscore replacer: returning '%s'\n",output.c_str());
    return output;
};

std::string cR (std::string input) //replaces \n with a symbol
{
    std::string output;
    output.assign(input);
    std::string ret;
    ret.assign("\u21B2");
    std::string car;
    car.assign( "\u000A");

    while(output.find("\\n")!=std::string::npos)
    {
        //printf("'%c' '%c'\n", output[output.find('\n')], output[output.find('\n')+2]);
        output.replace(output.find("\\n"),2,ret.c_str());
        //printf("current output:'%s'\n",output.c_str());
    };

    //printf("underscore replacer: returning '%s'\n",output.c_str());
    return output;
};

void cprintf(std::string input)
{
    int i=0;
    std::string control;
    control.assign("ab");
    control.replace(0,1,"\\");
    control.replace(1,1,"n");
    while (i<=input.length())
    {
        //printf("\n %c, %c, %i \n",input[i],control[0],input[i]==control[0]);
        //printf("\n %c, %c, %i \n",input[i+1],control[1],input[i+1]==control[1]);
        if((input[i]==control[0])&&(input[i+1]==control[1])){printf("\n");i++;}
        else{printf("%c",input[i]);};
        i++;
    };
};

std::string seedR (std::string input, int seed)
{
    std::string output;
    output.assign(input);
    char buffer[2];
    //printf("%c %i\n",buffer, seed);
    snprintf(buffer,sizeof(char)*2,"%i",seed);
    //printf("%c\n",buffer[0]);
    std::replace(output.begin(), output.end(), '#', buffer[0]);
    //printf("underscore replacer: returning '%s'\n",output.c_str());
    return output;
};

std::string windowsFixer (std::string input) //removes trailing /r from files
{
    std::string output;
    output.assign(input);
    std::replace(output.begin(), output.end(), '\r', ' ');
    //printf("underscore replacer: returning '%s'\n",output.c_str());
    return output;

};

typedef enum {cm_move, cm_load, cm_oload, cm_mload, cm_save, cm_delete, cm_roll, cm_show, cm_reset, cm_exit, cm_help, cm_previous, cm_sumroll,
cm_crmove, cm_crload, cm_crdelete, cm_crroll, cm_crshow, cm_crmake, cm_credit,
cm_editinsert, cm_editmove, cm_editdelete, cm_editreplace,
cm_uncontested, cm_contested, cm_clash,
cm_pix,cm_stoggle,
cm_total, cm_invalid} cm_input;
cm_input CMCV(std::string const& input)
{
    cm_input output;
    output=cm_invalid;
    if((input=="move")||(input=="m")){output=cm_move;};
    if((input=="load")||(input=="l")){output=cm_load;};
    if((input=="oload")||(input=="ol")){output=cm_oload;};
    if((input=="mload")||(input=="ml")){output=cm_mload;};
    if(input=="save"){output=cm_save;};
    if((input=="delete")||(input=="del")){output=cm_delete;};
    if((input=="reset")||(input=="res")){output=cm_reset;};
    if((input=="roll")||(input=="r")){output=cm_roll;};
    if((input=="sroll")||(input=="sr")){output=cm_sumroll;};
    if((input=="show")||(input=="s")){output=cm_show;};
    if((input=="exit")||(input=="x")){output=cm_exit;};
    if((input=="help")||(input=="h")){output=cm_help;};
    if((input=="previous")||(input=="p")||(input=="")){output=cm_previous;};
    if((input=="crmove")||(input=="crm")){output=cm_crmove;};
    if((input=="crload")||(input=="crl")){output=cm_crload;};
    if((input=="crdelete")||(input=="crdel")){output=cm_crdelete;};
    if((input=="croll")||(input=="cr")){output=cm_crroll;};
    if((input=="crshow")||(input=="crs")){output=cm_crshow;};
    if(input=="crmake"){output=cm_crmake;};
    if(input=="credit"){output=cm_credit;};
    if((input=="ei")||(input=="einsert")){output=cm_editinsert;};
    if((input=="er")||(input=="ereplace")){output=cm_editreplace;};
    if((input=="ed")||(input=="edelete")){output=cm_editdelete;};
    if((input=="em")||(input=="emove")){output=cm_editmove;};
    if((input=="uncon")||(input=="ncon")||(input=="uncontested")){output=cm_uncontested;};
    if((input=="con")||(input=="contested")){output=cm_contested;};
    if((input=="cl")||(input=="clash")){output=cm_clash;};
    if((input=="pix")){output=cm_pix;};
    if((input=="stoggle")){output=cm_stoggle;};

    //printf("CMCV OUTPUT: %i\n",output);
    return output;
};



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////// DICE CLASS ///////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct face{
    std::string content;
    struct face* next;
} face;

class dieclass{

protected:
    std::string name;
    int faces;
    face* facelist;

public:
    void setname(std::string text); //DONE
    void setname();
    std::string getname(); //DONE

    void setface(int facenumber, std::string text); //DONE
    std::string getface (int facenumber);
    void delface(int facenumber);

    void setfacenumber (int number);//ALSO BUILDS AN EMPTY LIST
    int getfacenumber();//DONE

    std::string roll (int times, int BoonsBanes, int bonus, bool croll); //DONE
    void showdie(); //DONE

    face* buildface(face* listptr, std::string text);//also add 1 to faces; mallocs, builds face, returns pointer to new face (to proceed with building). if ptr=facelist special behaviour

    dieclass(); // Standard constructor
    dieclass(std::string filename); //constructor overload: read by file test
    dieclass(fstream* myfile); //second constructor overload: read until $ test
    ~ dieclass(); //X
};

dieclass::dieclass(){name="EMPTY NAME"; faces=0; facelist=NULL;};
dieclass::~dieclass()
{
    //printf("deleting die %s\n",name.c_str());
    name.clear();
    faces=0;
    face* templist=facelist;
    face* tempp=NULL;
    if (facelist==NULL){return;};
    do
    {
        templist->content.clear();
        tempp=templist;
        templist=templist->next;
        tempp->next=NULL;
        delete tempp;
        //printf("face deleted\n");
    }while (templist!=NULL);
    facelist=NULL;
    //printf("die deleted\n\n");
};

void dieclass::setname(std::string text){name.assign(text);};
void dieclass::setname(){name.clear();};
std::string dieclass::getname(){return name;};

int dieclass::getfacenumber(){return faces;};
void dieclass::setfacenumber(int number)
{
    faces=number;
    if (facelist!=NULL){printf("SETFACENUMBER: DIE LIST ALREADY EXISTS\n");return;};
    if (number==0){printf("SETFACENUMBER: Can't create a die with 0 faces\n");return;};
    face* tempp=NULL;
    // BUILD FIRST ELEMENT, AND GIVE IT TO FACELIST
    tempp=(face*)malloc(sizeof(face));
    tempp->content="EMPTY";
    tempp->next=NULL;
    facelist=tempp;
    number-=1;

    while (number!=0) {
            tempp->next=(face*)malloc(sizeof(face));
            tempp=tempp->next;
            tempp->content="EMPTY";
            tempp->next=NULL;
            number-=1;
    };

    //DEBUG
    /*
    tempp=facelist;
    printf("LIST DEBUG\n");
    do{
        printf("%p %s %p\n", tempp, tempp->content.c_str(), tempp->next);
        tempp=tempp->next;
    }while (tempp!=NULL);
    */
    tempp=NULL;
    return;
};

void dieclass::setface(int facenumber, std::string text)
{
    if(facelist==NULL){printf("SETFACE: build die first with setfacenumber\n");return;};
    if(facenumber>faces){printf("SETFACE: index out of bounds \n");return;};
    face* tempp= facelist;
    int i=1;
    while (i!= facenumber)
    {
        tempp=tempp->next;
        i++;
    };
    tempp->content.assign(text);

};

std::string dieclass::getface(int facenumber)
{
    face* tempp= facelist;
    if(facenumber>faces){/*printf("GETFACE: index out of bounds\n");*/return "GETFACE: ERROR";};
    int i=1;
    while (i!= facenumber)
    {
        tempp=tempp->next;
        i++;
    };
    return tempp->content;
};

std::string dieclass::roll(int times=1, int BoonsBanes=0, int bonus=0, bool croll=false)
{
    if (croll==false){printf("ROLLING %s %i TIMES...\n\n", uR(getname()).c_str(), times);};
    int i;
    int rng,temprng;
    for (i=1;i<=times;i++)
    {
        rng=rand() %getfacenumber() +1;
        //printf("\n RNG: %i",rng);
        if (BoonsBanes!=0)
        {
            temprng=rand() %getfacenumber() +1;
            if (BoonsBanes>0)
            {
                rng=max(rng,temprng);
                BoonsBanes-=1;
            }else
            {
                rng=min(rng,temprng);
                BoonsBanes+=1;
            };
            //printf(" TEMPRNG:%i, --> RNG:%i",temprng,rng);
        };
        //printf(" BONUS %i", bonus);
        rng+=bonus;
        if(rng>getfacenumber()){rng=getfacenumber();};
        if(rng<1){rng=1;};
        //printf(" TOTAL: %i\n",rng);

        if (croll==false){printf("%s\n",getface(rng).c_str());};
    };
    return getface(rng).c_str();
};

void dieclass::showdie()
{
    if(facelist==NULL){printf("SHOWDIE: DIE IS EMPTY\n");return;};
    printf("NAME: %s (%d)\n", uR(getname()).c_str(),faces);
    int i;
    for (i=1;i<=getfacenumber();i++)
    {
        printf("%i: %s\n",i, getface(i).c_str());
    };
    return;
};

face* dieclass::buildface(face* listptr, std::string text)
{
    faces+=1;
    face tempptr{};
    tempptr.content.assign(text);
    tempptr.next=NULL;
    if (listptr==NULL) //you're passing an empty list
    {
        /*
        printf("passed an empty facelist: listptr %p\n",listptr);
        facelist= (face*)malloc(sizeof(face));
        printf("facelist %p %s %p \n",facelist, facelist->content.c_str(),facelist->next);
        facelist->content.assign(text);
        printf("facelist %p %s %p \n",facelist, facelist->content.c_str(),facelist->next);
        facelist->next=NULL;
        */
        facelist=new face{};
        (*facelist)=tempptr;
         //printf("returning: facelist %p %s %p \n",facelist, facelist->content.c_str(),facelist->next);
        return facelist;
    }
    else
        {
            /*
            printf("passed an unempty facelist: listptr %p\n",listptr);
            listptr->next=(face*)malloc(sizeof(face));
            printf("listptr %p %s %p \n",listptr, listptr->content.c_str(),listptr->next);
            listptr=listptr->next;
            printf("listptr %p %s %p \n",listptr, listptr->content.c_str(),listptr->next);
            listptr->content.assign(text);
            printf("listptr %p %s %p \n",listptr, listptr->content.c_str(),listptr->next);
            listptr->next=NULL;
            */
            listptr->next=new face{};
            listptr=listptr->next;
            (*listptr)=tempptr;
            //printf("returning: listptr %p %s %p \n",listptr, listptr->content.c_str(),listptr->next);
            return listptr;
        };
};

dieclass::dieclass(std::string filename) //constructor overload
{
    faces=0; name='\0'; facelist=NULL;

    fstream myfile;
    myfile.open(filename, ios::in);

    std::string tempstring;
    face* tempptr=NULL;
    if(!myfile.is_open()){printf("CONSTRUCTOR: Error opening file %s",filename.c_str());return;};


    getline(myfile, tempstring);
    tempstring.assign(windowsFixer(tempstring));
    //tempstring.assign(trimstring(tempstring));
  //  printf("GOT STRING: %s; %d\n", tempstring.c_str(), tempstring.find("$")==std::string::npos);

    while(tempstring.find("$")==std::string::npos)
    {

       // printf("Inserting: %s\n",tempstring.c_str());
        tempptr=buildface(tempptr,tempstring);
        tempstring.assign("");
        getline(myfile, tempstring);
        tempstring.assign(windowsFixer(tempstring));
        //tempstring.assign(trimstring(tempstring));
       // printf("GOT STRING: %s\n\n", tempstring.c_str());
    };
    tempptr=NULL;
    myfile.close();

    return;
};

dieclass::dieclass(fstream* myfile) //constructor overload
{
    faces=0; name='\0'; facelist=NULL;


    std::string tempstring;
    face* tempptr=NULL;
    if(!(*myfile).is_open()){printf("CONSTRUCTOR 2: STREAM OF FILE IS NOT OPEN");return;};


    getline(*myfile, tempstring);
    tempstring.assign(windowsFixer(tempstring));
    //tempstring.assign(trimstring(tempstring));
   // printf("GOT STRING: %s; %d\n", tempstring.c_str(), tempstring.find("$")==std::string::npos);

    while(tempstring.find("$")==std::string::npos)
    {

       // printf("Inserting: %s\n",tempstring.c_str());
        tempptr=buildface(tempptr,tempstring);
        tempstring.assign("");
        getline(*myfile, tempstring);
        tempstring.assign(windowsFixer(tempstring));
        //tempstring.assign(trimstring(tempstring));
       // printf("GOT STRING: %s\n\n", tempstring.c_str());
    };
    tempptr=NULL;

    return;
};

void dieclass::delface(int facenumber)
{
    if(facelist==NULL){printf("DEL FACE: list is empty\n");return;};
    face* templist=facelist;
    face* templist2=NULL;
    if (facenumber>getfacenumber()){printf("DELETE: target out of bounds; operation aborted\n");return;};

    if(facenumber==1)//delete the first die;
    {
        //printf("deleting first die\n");
        facelist=facelist->next;
        templist->content.clear();
        delete templist;
    }
    else{
            while (facenumber>2)
            {
                templist=templist->next;
                facenumber--;
            };//now points to the one BEFORE it

            templist2=templist->next;
            templist->next=templist2->next;

            templist2->content.clear();
            delete templist2;
        };

    faces-=1;

    templist=NULL;
    templist2=NULL;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////ROLLER CLASS////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
using entry=face; //alias for face (which is a content+pointer)

class rollerclass: public dieclass
{
public:

    // set/getname remains the same

    void setentriestotal(int number){return setfacenumber(number);};
    int getentriestotal(){return getfacenumber();};

    void setentry(int rollentry, std::string text) {setface(rollentry, text);};
    std::string getentry(int rollentry) {return getface(rollentry);};
    void delentry(int entrynumber){delface(entrynumber);};

    void showroller();
    void roll(int times, int BoonsBanes, int bonus){printf("rollerclass has roll method disabled; programstate will access it\n");};

    entry* buildentry(entry* listptr, std::string text) {return buildface(listptr, text);};
    bool isroll(std::string content); //X

    ~rollerclass();
    using dieclass::dieclass;

};

rollerclass::~rollerclass()
{
    //printf("deleting die %s\n",name.c_str());
    name.clear();
    faces=0;
    face* templist=facelist;
    face* tempp=NULL;
    if (facelist==NULL){return;};
    do
    {
        templist->content.clear();
        tempp=templist;
        templist=templist->next;
        tempp->next=NULL;
        delete tempp;
        //printf("face deleted\n");
    }while (templist!=NULL);
    facelist=NULL;
    //printf("die deleted\n\n");
};

bool rollerclass::isroll(std::string content)
{
    content.assign(trimstring(content));
    content.assign(seedR(content,1));
    arglist myarglist=getargs(content);
    bool commRoll=((myarglist.command.compare("roll")==0)||(myarglist.command.compare("r")==0)||(myarglist.command.compare("sroll")==0)||(myarglist.command.compare("sr")==0));
    bool arg1AndDcheck=(((myarglist.arg1!=0)^((myarglist.args[0]=='d')&&(isdigit(myarglist.args[1])!=0)))||(myarglist.arg1==-6));
    bool rollcheck=((commRoll==true)&& (arg1AndDcheck==true) && (myarglist.arg2>0));

    bool commUncon=((myarglist.command.compare("ncon")==0)||(myarglist.command.compare("uncon")==0)||(myarglist.command.compare("uncontested")==0));
    bool unconArgs=((myarglist.arg1>0)&&(myarglist.arg3>=1));
    bool unconcheck=((commUncon==true)&&(unconArgs==true));
    //contested and clash have the same requirements
    bool commCC=((myarglist.command.compare("contested")==0)||(myarglist.command.compare("con")==0)||(myarglist.command.compare("clash")==0)||(myarglist.command.compare("cl")==0));
    bool CCArgs=((myarglist.arg1>0)&&(myarglist.arg3>0));
    bool CCcheck=((commCC==true)&&(CCArgs==true));

    //printf("ISROLL: checking args: '%s' %i %i %i %i '%s'\n",myarglist.command.c_str(),myarglist.arg1,myarglist.arg2,myarglist.arg3,myarglist.arg4,myarglist.args.c_str());
    //printf("commroll:%i, arg1&d:%i\n",commRoll==true, arg1AndDcheck==true);
    //printf("ISROLL: rollcheck:%i, unconcheck:%i, CCcheck:%i\n",rollcheck==true,unconcheck==true, CCcheck==true);
    //getchar();



    //must start with "roll" and have either an args (dx) or an arg1, and an arg2


    if ((rollcheck==true)||(unconcheck==true)||(CCcheck==true))
    {return true;}
    else
    {return false;};
};

void rollerclass::showroller()
{


    printf("NAME: %s\n", uR(getname()).c_str());
    int i=1;
    arglist myargs{};
    if (facelist==NULL){printf("SHOWROLLER: list is empty\n");return;};

    do
    {
        if(isroll(getentry(i))==false)
        {
            if(getentry(i)=="&&")
            {
                printf("     *AND*\n");
            }
            else{
                cout<<cR(getentry(i))<<"\n";
                //cprintf(getentry(i));
                //printf("\n");
            };

        }
        else{
                myargs=getargs(getentry(i));
                switch(CMCV(myargs.command))
                {
                    case cm_sumroll: printf("SUM");
                    case cm_roll:
                        printf("   ROLL '%s'",uR(myargs.args).c_str()); //rollerlist will change index number -> dicename;
                        printf(" %i times", myargs.arg2);
                        printf(" +%i",myargs.arg3);
                        if (myargs.arg4>=0)
                        {
                            printf(" (%i boons)\n",myargs.arg4);
                        }
                        else{
                                printf(" (%i banes)\n",myargs.arg4);
                            };
                        break;

                    case cm_contested:
                        printf("   CONTESTED: %i(+%i) vs %i(+%i)\n", myargs.arg1, myargs.arg2,myargs.arg3,myargs.arg4);
                        break;

                    case cm_uncontested:
                        printf("   UNCONTESTED: %i(+%i) vs DC: %i\n", myargs.arg1, myargs.arg2,myargs.arg3);
                        break;

                    case cm_clash:
                        printf("   CLASH: %i(+%i) vs %i(+%i)\n", myargs.arg1, myargs.arg2,myargs.arg3,myargs.arg4);
                        break;

                    default: printf("SHOWROLLER: error parsing the roll format\n");
                };

            };
        i++;
    }while(i<= getentriestotal());


};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////// DICELIST CLASS ///////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


typedef struct onedie{
union{dieclass* die; rollerclass* roller;}; //so I can define oneroller* mylist and do mylist->roller
onedie* next;
}onedie;

using oneroller = onedie; //onedie alias;

class dielistclass{
protected:
    int dicenumber;
    onedie* dielist;

public:
    int getdicenumber(); //X
    void raisedicenumber(); //X
    onedie* getlist() {return dielist;};

    void showdice(bool debug); //X
    void showdice(int i);
    std::string rolldie(int whichdie, int times,  int bonus, int BoonsBanes, bool croll); //X
    int rolldie(std::string howmanyfaces, int times, int bonus, int BoonsBanes,  bool sum, bool croll); //X
    void uncontested(int atk, int modif,int CD);
    void contested(int atk, int amodif, int def, int dmodif);
    void clash(int atk, int abonus, int def, int dbonus);

    void loadlist(); //X
    void loadmaster(std::string filename); //X
    void savemaster(std::string filename); //X
    void savedummy(std::string filename);

    void putdie(int dienumber, int position); //X
    void append(std::string filename); //X
    void deldie (int dienumber);

    int getdieposition(std::string dicename);
    onedie* getdie (int i);
    onedie* checkdie(std::string dicename);

    dielistclass(); //X
    dielistclass(std::string filename); //X
    ~dielistclass(); //X

};

int dielistclass::getdicenumber(){return dicenumber;};
void dielistclass::raisedicenumber(){dicenumber+=1;return;};
dielistclass::dielistclass(){dicenumber=0;dielist=NULL;};
dielistclass::~dielistclass()
{
    dicenumber=0;
    onedie* templist=dielist;
    onedie* tempp=NULL;
    //printf("deleting dielistclass \n");

    while (templist!=NULL)
    {
       // printf("die deletion \n");
        delete templist->die;
        tempp=templist;
        templist=templist->next;
        tempp->next=NULL;
        free(tempp);
    };
    dielist=NULL;
   // printf("dielistclass deleted\n\n");

};
dielistclass::dielistclass(std::string filename)
{
    fstream myfile;
    std::string mystring;
    dicenumber=0;dielist=NULL;
    onedie* templist=NULL;
    face* tempface=NULL;


    myfile.open(filename,ios::in);
    if(!myfile.is_open()){printf("DLC CONSTRUCTOR: Error opening file\n");perror("Error:");return;};

    //OPLIST: 1) check endfile 2)ELSE: read 1st line->name 3)keep reading until $-> dice REPEAT
    while ((myfile.eof()==false)&&(myfile.peek()!='$')){

            if (dielist==NULL)
            {
                //printf("dielist is empty, creating dielist \n");
                dielist=(onedie*)malloc(sizeof(onedie));
                templist=dielist;
                //printf("DIELIST %p TEMPLIST %p TEMPLIST NEXT %p\n", dielist, templist, templist->next);
            }
            else{
                //printf("dielist is not empty, generating new dielist element\n");
                templist->next=(onedie*)malloc(sizeof(onedie));
                templist=templist->next;
            };
            templist->next=NULL;
            templist->die=new dieclass;
            //printf("DIELIST %p TEMPLIST %p TEMPLIST NEXT %p\n", dielist, templist, templist->next);

            mystring.clear();
            getline(myfile, mystring);
            mystring.assign(windowsFixer(mystring));
            mystring.assign(trimstring(mystring));
            templist->die->setname(mystring);

            mystring.clear();
            getline(myfile,mystring);
            mystring.assign(windowsFixer(mystring));
            //mystring.assign(trimstring(mystring));
            //printf("GOT STRING: %s\n\n", mystring.c_str());

            while(mystring.find("$")==std::string::npos)
            {
                //printf("Inserting: %s\n",mystring.c_str());
                tempface=templist->die->buildface(tempface,mystring);
                mystring.clear();
                getline(myfile, mystring);
                mystring.assign(windowsFixer(mystring));
                //mystring.assign(trimstring(mystring));
                //printf("GOT STRING: %s\n\n", mystring.c_str());
            };

            tempface=NULL;
            raisedicenumber();
            myfile.peek();

    };

    templist=NULL;
    myfile.close();

};

void dielistclass::append(std::string filename)
{
    fstream myfile;
    std::string mystring;
    onedie* templist=NULL;
    face* tempface=NULL;

    //printf("filename %s\n",filename.c_str());
    myfile.open(filename,ios::in);
    if(!myfile.is_open()){printf("APPEND: Error opening file\n");perror("Error:");return;};

    while ((myfile.eof()==false)&&(myfile.peek()!='$')){

            //printf("\n DIELIST=NULL? %i\n", dielist==NULL);
            if (dielist==NULL)
            {
                    // build the first element and templist to it
                dicenumber=0; //standard initialization you never know
                //printf("dielist is empty, creating dielist \n");
                dielist=(onedie*)malloc(sizeof(onedie));
                templist=dielist;
                //printf("FIRST TIME TEST: DIELIST %p TEMPLIST %p TEMPLIST NEXT %p\n\n", dielist, templist, templist->next);
            }
            else
            {
                    //printf("dielist is not empty, scrolling until the last element \n");
                    //scroll until the last element then initialize to it
                    templist=dielist;
                    while (templist->next!=NULL)
                    {
                        //printf("SCROLLING DIELIST... %p: TEMPLIST: %p NEXT:%p\n", dielist,templist,templist->next);
                        templist=templist->next;
                    };
                    //printf("CURRENT: TEMPLIST: %p NEXT:%p\n",templist,templist->next);
                    templist->next=(onedie*)malloc(sizeof(onedie));
                    templist=templist->next;
            };




            templist->next=NULL;
            templist->die=new dieclass;
            //printf("HAVE BUILT NEW DIE: DIELIST %p TEMPLIST %p TEMPLIST NEXT %p\n\n", dielist, templist, templist->next);

            mystring.clear();
            getline(myfile, mystring);
            mystring.assign(windowsFixer(mystring));
            mystring.assign(trimstring(mystring));
            templist->die->setname(mystring);
            //printf("append: GOT NAME: %s\n", mystring.c_str());

            mystring.clear();
            getline(myfile,mystring);
            mystring.assign(windowsFixer(mystring));
            //mystring.assign(trimstring(mystring));
            //printf("GOT STRING: %s\n", mystring.c_str());

            while(mystring.find("$")==std::string::npos)
            {
                //printf("Inserting: %s\n\n",mystring.c_str());
                tempface=templist->die->buildface(tempface,mystring);
                mystring.clear();
                getline(myfile, mystring);
                mystring.assign(windowsFixer(mystring));
                //mystring.assign(trimstring(mystring));
                //printf("GOT STRING: %s\n", mystring.c_str());
            };

            tempface=NULL;
            raisedicenumber();
            //printf("current dicenumber %i\n\n",dicenumber);
            myfile.peek();

    };

    myfile.close();

};


void dielistclass::showdice(bool debug=false)
{
    //printf("\n LIST HAS %i DICE\n\n",getdicenumber());
    printf("===== DICE LIST =====\n");
    if (getdicenumber()==0){return;};
    onedie* templist=dielist;
    int i=1;
    do
    {
        if (debug==true){printf("%p  \n",templist);};
        //printf("%i) %s (%i)",i, uR(templist->die->getname()).c_str(), templist->die->getfacenumber());
        cout<<i<<") "<<uR(templist->die->getname());
        cout<<" ("<<templist->die->getfacenumber()<<")\n";
        if (debug==true){printf(" NEXT:%p",templist->next);};
        //printf("\n");
        if (debug==true){templist->die->showdie();printf("\n");};

        i++;
        templist=templist->next;

    }while (templist!=NULL);
    printf("\n");
};

void dielistclass::showdice (int i)
{
    if(dielist==NULL){printf("SHOWDICE: list is empty\n");return;};
    if ((i<1)||(i>getdicenumber())){/*printf("SHOWDICE: index out of bounds/n");*/ return;};
    onedie* templist=dielist;

    while(i>1)
    {
        templist=templist->next;
        i--;
    };

    templist->die->showdie();
    templist=NULL;
};

void dielistclass::loadlist()
{
    std::string filename;
    for (const auto & entry : fs::directory_iterator("res/dice"))
    {
        filename.clear();
        filename.assign(entry.path());
        printf("LOADING %s\n",filename.c_str());
        append(filename);
    };
};

void dielistclass::putdie(int dienumber, int position)
{
    if(dielist==NULL){printf("PUTDIE list is empty\n");return;};
    onedie* temp1=dielist;
    onedie* temp2=dielist;

    if((dienumber<1)||(dienumber>getdicenumber())){printf("\nPUTDIE ERROR: dienumber out of bounds\n");return;};
    if((position<1)||(position>getdicenumber())){printf("\nPUTDIE ERROR: position out of bounds\n");return;};


    if (dienumber==1)
    {
        dielist=dielist->next;//automatic sewing, now the die to swap is the first, and it's in temp2
    }else
    {
        while(dienumber>2)
        {
            temp1=temp1->next;
            dienumber--;
        };//now temp1 points to the one before the right one


        temp2=temp1->next;//temp2 points to the correct one
        temp1->next=temp2->next; //sewing back the list
        temp2->next=NULL; //nulling the swapping one's pointer
    };

    temp1=dielist;

    if (position==1)
    {
        dielist=temp2;
        temp2->next=temp1;
    }else
    {
        while(position>2)
        {
            temp1=temp1->next;
            position--;
        };//again points to the one before
        temp2->next=temp1->next;
        temp1->next=temp2;
    };
    temp1=NULL;
    temp2=NULL;
};

void dielistclass::savedummy(std::string filename)
{
    remove(filename.c_str());
    fstream outputfile;
    printf("savedummy: saving to file %s\n",filename.c_str());
    outputfile.open(filename.c_str(),ios::out);
    outputfile<<"$$"<<endl;
    outputfile.close();
};

void dielistclass::savemaster(std::string filename)
{
    remove(filename.c_str());
    fstream outputfile;
    printf("savemaster: saving to file %s\n",filename.c_str());
    outputfile.open(filename.c_str(),ios::out);
    onedie* temp1=dielist;
    int i=1;


    if (outputfile.is_open()==true)
    {
        printf("saving to file...\n");
        do{ //print name \n, print faces \n, print $ \n
                outputfile<< temp1->die->getname() <<endl;

                i=1;
                do{
                    outputfile<< temp1->die->getface(i)<<endl;
                    i++;
                }while (i<=temp1->die->getfacenumber());

                outputfile<<"$";
                temp1=temp1->next;
                if(temp1==NULL)
                {outputfile<<"$";
                }else
                {outputfile<<endl;};

        }while (temp1!=NULL);
    }
    else
    {
        perror("File error:");
    };
    outputfile.close();
    temp1=NULL;
};

void dielistclass::loadmaster(std::string filename)
{
    //std::string path="res/saves/";
    //path.append(filename);
        //append(path);
        append(filename);
};

std::string dielistclass::rolldie(int whichdie, int times=1, int bonus=0, int BoonsBanes=0, bool croll=false)
{
    if(dielist==NULL){printf("ROLLDIE list is empty\n");return "";};
    if((whichdie<=0)||(whichdie>dicenumber)){printf("ROLLDIE: dienumber out of bounds\n");return "";};
    if(times<0){printf("ROLLDIE: number of rolls out of bounds\n");return "";};
    std::string output;
    output.clear();

    onedie* tempptr=dielist;
    while(whichdie>1)
    {
        tempptr=tempptr->next;
        whichdie--;
    };

    output.assign(tempptr->die->roll(times, BoonsBanes, bonus,croll));
    if (croll==false){printf("\n");};
    return output;

};

int dielistclass::rolldie(std::string howmanyfaces, int times=1, int bonus=0, int BoonsBanes=0,  bool sum= false, bool croll=false)
{
    int partial=0;
    if ((int)howmanyfaces.find_first_of('d')!=0)
    {
        printf("%i \n",(int)howmanyfaces.find_first_of('d'));
        printf("ROLLDIE: not a dX roll: %s \n",howmanyfaces.c_str());
        return -1;
    };
    howmanyfaces.erase(0,1);
    //printf("faces: %s\n",howmanyfaces.c_str());

    if(times<0){printf("ROLLDIE: number of rolls out of bounds\n");return -1;};
    int faces=atoi(howmanyfaces.c_str());

    int rng,temprng;
    while (times>0)
    {
        rng=rand() %faces +1;
        if (croll==false){printf("ROLLED: %i",rng);};
        if (BoonsBanes!=0)
        {
            temprng=rand() %faces +1;
            if (BoonsBanes>0)
            {
                rng=max(rng,temprng);
                BoonsBanes-=1;
                if (croll==false){printf(" (BOONDIE: ");};
            }else
            {
                rng=min(rng,temprng);
                BoonsBanes+=1;
                if (croll==false){printf(" (BANEDIE: ");};
            };
            if (croll==false){printf(" %i)-> ROLL: %i",temprng,rng);};
        }
        if (croll==false){printf(" + %i",bonus);};
        rng+=bonus;
        if (croll==false){printf(" ===> %i\n",rng);};
        times--;
        if(sum==true){partial+=rng;};
    };
    if (croll==false){printf("\n");};
    if (croll==false){if(sum==true){printf("TOTAL: %i\n",partial);};};
    if (sum==true){return partial;}else{return rng;};
};

void dielistclass::uncontested (int atk, int modif, int CD)
{
    int i,rng;
    int amax=1;
    int rolled6=-1;
    printf("(");
    for (i=1;i<=atk;i++)
    {
        rng=rand()%6 +1;
        if (i!=1){printf(", ");};
        printf("%i",rng);
        amax=max(amax,rng);
        if(rng==6){rolled6+=1;};
        if((rolled6>0)&&(rng==6)){printf("*");};
    };
    if(rolled6<0){rolled6=0;};
    printf("// +%i +%i*)  >  %i    ***VS***   CD %i\n\n Degree of Success: %i\n",modif,rolled6,amax+modif+rolled6,CD,amax+modif+rolled6-CD);
};

void dielistclass::contested(int atk, int amodif, int def, int dmodif)
{
    int i,rng;
    int amax=1;
    int dmax=1;
    int arolled6=-1;
    int drolled6=-1;
    printf("(");
    for (i=1;i<=atk;i++)
    {
        rng=rand()%6 +1;
        if (i!=1){printf(", ");};
        printf("%i",rng);
        amax=max(amax,rng);
        if(rng==6){arolled6+=1;};
        if((arolled6>0)&&(rng==6)){printf("*");};
    };
    if(arolled6<0){arolled6=0;};
    printf("//+%i +%i*)  >  %i\n\n        ***VS***\n\n(",amodif,arolled6,amax+amodif+arolled6);

    for (i=1;i<=def;i++)
    {
        rng=rand()%6 +1;
        if (i!=1){printf(", ");};
        printf("%i",rng);
        dmax=max(dmax,rng);
        if(rng==6){drolled6+=1;};
        if((drolled6>0)&&(rng==6)){printf("*");};
    };
    if(drolled6<0){drolled6=0;};
    printf("//+%i +%i*)  >  %i\n\n Degree of Success: %i\n",dmodif,drolled6,dmax+dmodif+drolled6, amax+amodif+arolled6-dmax-dmodif-drolled6);
};

void dielistclass::clash(int atk, int abonus, int def, int dbonus)
{
    int i;
    int ares,dres;
    int rounds=max(atk,def);

    for(i=1;i<=rounds;i++)
    {
        ares=dres=0;
        printf("CLASH %i : ",i);
        if(atk>0){ares=rand()%6+1;printf("%i",ares);}else{ares=0;printf("--");};
        printf("(+%i) ***VS*** ",abonus);
        if(def>0){dres=rand()%6+1;printf("%i",dres);}else{dres=0;printf("--");};
        printf("(+%i) ====== Degree of Success: %i\n",dbonus,ares+abonus-dres-dbonus);
        atk-=1;def-=1;
    };

};

void dielistclass::deldie (int dienumber)
{
    if(dielist==NULL){printf("DELETE: list is empty\n");return;};
    onedie* templist=dielist;
    onedie* templist2=NULL;
    if (dienumber>getdicenumber()){printf("DELETE: target out of bounds; operation aborted\n");return;};

    if(dienumber==1)//delete the first die;
    {
        //printf("deleting first die\n");
        dielist=dielist->next;
        delete templist->die;
        delete templist;
    }
    else{
            while (dienumber>2)
            {
                templist=templist->next;
                dienumber--;
            };//now points to the one BEFORE it

            templist2=templist->next;
            templist->next=templist2->next;

            delete templist2->die;
            delete templist2;
        };

    dicenumber-=1;

    templist=NULL;
    templist2=NULL;
};

onedie* dielistclass::checkdie(std::string dicename)
{
    onedie* templist=dielist;

    while ((templist!=NULL)&&(templist->die->getname().compare(dicename)!=0))
    {
        //printf("comparing dicename:'%s' with inlist:'%s'\n",dicename.c_str(),templist->die->getname().c_str());
        templist=templist->next;
    };
    return templist;
};

int dielistclass::getdieposition(std::string dicename)
{
    onedie* templist=dielist;
    int i=1;

    while ((templist!=NULL)&&(templist->die->getname().compare(dicename)!=0))
    {
        //printf("loop %i templist %p >",i,templist);
        templist=templist->next;
        i++;
       //printf("GETDIEPOS: %i   TEMPLIST: %p \n",i,templist);
    };

    //printf("out of loop\n");

    if(templist==NULL){return 0;} else {return i;};
};

onedie* dielistclass::getdie(int i)
{
    onedie* tempp=dielist;
    int j=1;

    if((i<=0)||(i>getdicenumber())){/*printf("GETDIE: index out of bounds\n");*/return NULL;};

    if (i==1){return dielist;};

    for(j=1;j<i;j++)
    {
        tempp=tempp->next;
    };

    return tempp;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////// ROLLERLIST CLASS /////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class rollerlistclass: public dielistclass{

public:
    oneroller* rlist(){return dielist;};
    int getrollernumber(){getdicenumber();};
    void raiserollernumber(){raisedicenumber();};
    oneroller* getroller(int i);

    void showrollers(bool debug);
    void showrollers(int i);
    void rolldie(int whichdie, int times,  int bonus, int BoonsBanes) {printf("Rolldie method is disabled for rollerlist class\n");return;};
    void rolldie(std::string howmanyfaces, int times, int bonus, int BoonsBanes,  bool sum) {printf("Rolldie method is disabled for rollerlist class\n");return;};

    void loadlist(); //works from a different directory
    //void loadmaster(std::string filename); //Same method. Parser will take it from a different directory
    //void savemaster(std::string filename); //Same method. Parser will take it from a different directory

    void putroller(int dienumber, int position) {putdie(dienumber,position);};
    //void append(std::string filename); append is the same method
    void delroller (int dienumber) {deldie(dienumber);};

    int getdieposition(std::string dicename){printf("getdie method is disabled for rollerlist class\n");return 0;};
    onedie* checkdie(std::string dicename) {printf("checkdie method is disabled for rollerlist class\n");return NULL;};

    using dielistclass::dielistclass;
    ~rollerlistclass();

};

rollerlistclass::~rollerlistclass()
{
    dicenumber=0;
    onedie* templist=dielist;
    onedie* tempp=NULL;
    //printf("deleting dielistclass \n");

    while (templist!=NULL)
    {
       // printf("die deletion \n");
        delete templist->die;
        tempp=templist;
        templist=templist->next;
        tempp->next=NULL;
        free(tempp);
    };
    dielist=NULL;
   // printf("dielistclass deleted\n\n");

};

void rollerlistclass::loadlist()
{
    std::string filename;
    for (const auto & entry : fs::directory_iterator("res/rollers"))
    {
        filename.clear();
        filename.assign(entry.path());
        printf("LOADING %s\n",filename.c_str());
        append(filename);
    };
};

void rollerlistclass::showrollers(bool debug=false)
{
    //printf("\n LIST HAS %i DICE\n\n",getdicenumber());
    printf("===== ROLLERS LIST =====\n");
    if (getdicenumber()==0){return;};
    onedie* templist=dielist;
    int i=1;
    do
    {
        if (debug==true){printf("%p  \n",templist);};
        printf("%i) %s (%i)",i, uR(templist->die->getname()).c_str(), templist->die->getfacenumber());
        if (debug==true){printf(" NEXT:%p",templist->next);};
        printf("\n");
        if (debug==true){templist->die->showdie();printf("\n");};

        i++;
        templist=templist->next;

    }while (templist!=NULL);
    printf("\n");
};

void rollerlistclass::showrollers (int i)
{
    if(dielist==NULL){printf("SHOWROLLER: list is empty\n");return;};
    if ((i<1)||(i>getdicenumber())){printf("SHOWROLLER: index out of bounds/n"); return;};
    oneroller* templist=dielist;

    while(i>1)
    {
        templist=templist->next;
        i--;
    };

    templist->roller->showroller();
    templist=NULL;
};

oneroller* rollerlistclass::getroller(int i)
{
    oneroller* tempp=rlist();
    int j=1;

    if((i<=0)||(i>getrollernumber())){/*printf("GETROLLER: index out of bounds\n");*/return NULL;};

    if(i==1){return rlist();};

    for(j=1;j<i;j++)
    {
        tempp=tempp->next;
    };

    return tempp;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////PROGRAMSTATE CLASS////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
class programstate{

private:
        dielistclass* mylist;
        rollerlistclass* myrlist;
        std::string mystring;
        std::string savedcommand;
        bool exitflag;
        bool repeatflag;
        bool hideflag;

public:
        void parser(std::string input);
        void initialize();
        void resetlist();
        void resetRlist();
        void loop();

        dielistclass* getlist(){return mylist;};
        rollerlistclass* getrlist(){return myrlist;};

        std::string index_to_dicename (std::string content); //get a dicename from an dice index
        void rollerfixer(oneroller* target);
        void fix();
        void compatibility_check(); //purges noncompatible rollers from list

        void pixroll(int times);

        ~programstate();
        programstate();

};
programstate::programstate(){mylist=NULL; myrlist=NULL; mystring.clear(); savedcommand.clear(); exitflag=false; repeatflag=false; return;};
programstate::~programstate(){delete mylist; delete myrlist; mystring.clear(); savedcommand.clear();exitflag=true; repeatflag=false; return;};


void programstate::initialize(){mylist=new dielistclass(); myrlist=new rollerlistclass(); return;};
void programstate::resetlist()
{
    if(mylist!=NULL){delete mylist;};
    mylist=new dielistclass();
};

void programstate::resetRlist()
{
    if(myrlist!=NULL){delete myrlist;};
    myrlist=new rollerlistclass();
};

void programstate::pixroll(int times)
{
    if (times>10){times=10;};
    if (times<=0){times=3;};
    int i=1;
    int rng=0;
    char tempname[4];
    std::string filename;
    filename.clear();

    SDL_Surface* image=NULL;
    SDL_Rect dstrect;
    dstrect.x=0;
    dstrect.y=0;
    dstrect.h=IMGDIM;
    dstrect.w=IMGDIM;

    //create the window and get its surface
    window = SDL_CreateWindow( "PICTURE ROLL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_BORDERLESS );
    if( window == NULL ){printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );};
    windowSurface=SDL_GetWindowSurface(window);

    for (i=1;i<=times;i++)
    {
        filename.clear();
        filename.assign("res/pictures/");
        rng=rand()%totalpix +1;

        //printf("RNG:%i,",rng);
        snprintf(tempname, 4,"%i",rng);
        filename.append(tempname);
        filename.append(".png");
        //printf(" FILENAME:'%s'\n",filename.c_str());

        if(i<=5){dstrect.x=0+(i-1)*IMGDIM; dstrect.y=0;}
        else{dstrect.x=0+(i-6)*IMGDIM; dstrect.y=IMGDIM;};


        image=loadSurface(filename.c_str(),windowSurface);
        if(SDL_BlitScaled(image, NULL, windowSurface, &dstrect)!=0){printf("BLITSCALED ERROR:%s \n",SDL_GetError());};
        SDL_FreeSurface(image);
        image=NULL;

    };
    printf("press any key twice to return\n");
    SDL_UpdateWindowSurface(window);
    getchar();
    SDL_FreeSurface(windowSurface);
    windowSurface=NULL;
    SDL_DestroyWindow(window);
    window=NULL;

};


void programstate::parser(std::string input)
{
    std::string command;
    std::string tempstring;
    arglist myargs,crargs;
    int i,j;
    i=j=0;

    bool arg5=false;


    tempstring.clear();

    myargs.arg1=myargs.arg2=myargs.arg3=myargs.arg4=0;
    myargs.args.clear();
    myargs.command.clear();

    myargs=getargs(input);

    //printf("GOT ARGS: '%s' '%i' '%i' '%i' '%i' '%s'\n",myargs.command.c_str(),myargs.arg1,myargs.arg2,myargs.arg3,myargs.arg4,myargs.args.c_str());





    switch (CMCV(myargs.command))
    {
        case cm_delete:
            //printf("delete a die\n");
            if(myargs.arg1<=0){printf("Delete is used 'delete X', where X is the number of the die \n");break;};
            //printf("got argument %s -> %i \n", tempstring.c_str(),arg1);
            mylist->deldie(myargs.arg1);
            compatibility_check();
            printf("Done. \n");
            break;


        case cm_load:
            printf("Enter number of dicelist to append, leave empty to go back\n");
            tempstring=fileselect("res/dice/");

            if(tempstring.compare("NOFILE")!=0)
            {
                mylist->append(tempstring);
                printf("Appended %s to list\n",tempstring.c_str());
            };
            break;

        case cm_oload:
            resetlist();
            printf("List reset\n");
            printf("Enter number of dicelist to load, leave empty to go back\n");
            tempstring=fileselect("res/dice/");

            if(tempstring.compare("NOFILE")!=0)
            {
                mylist->append(tempstring);
                printf("Loaded %s \n",tempstring.c_str());
            };
            compatibility_check();

            break;

        case cm_mload:
            resetlist();
            resetRlist();
            printf("List reset\n");
            printf("Enter number of master file to load, leave empty to go back\n\n");

            tempstring=fileselect("res/saves/");


            if(tempstring.compare("NOFILE")!=0)
            {
                mylist->loadmaster(tempstring);
                printf("List loaded\n");

                tempstring.insert(9,"_R");
                tempstring.append("_R");
                myrlist->loadmaster(tempstring);
                printf("Rollers loaded\n");

            };



            break;

        case cm_move:
            //printf("move a dice\n");
            //printf("got argument %s -> %i , %i \n", tempstring.c_str(),arg1, arg2);
            if((myargs.arg1<=0)||(myargs.arg2<=0)){printf("usage is move X Y, moves Xth die to become the Yth\n"); break;};
            if((myargs.arg1>mylist->getdicenumber())||(myargs.arg2>mylist->getdicenumber())){printf("indices out of bounds\n");break;};
            mylist->putdie(myargs.arg1,myargs.arg2);
            printf("Moved dice %i to position %i\n", myargs.arg1,myargs.arg2);
            break;

        case cm_save:
            if (mylist->getdicenumber()==0){printf("no dice to save\n");break;};
            printf("Enter filename to save\n");
            tempstring.clear();
            getline(cin,tempstring);
            if(tempstring.length()==0){printf("Aborted\n");break;}
            tempstring.insert(0,"res/saves/");
            mylist->savemaster(tempstring);
            printf("Saved %s \n",tempstring.c_str());


            tempstring.insert(9,"_R");
            tempstring.append("_R");
            //printf("TEST\n");
            if(myrlist==NULL){printf("no custom rollers to save\n"); myrlist->savedummy(tempstring); break;};
            if(myrlist->getrollernumber()==0){printf("no custom rollers to save\n"); myrlist->savedummy(tempstring); break;};
            myrlist->savemaster(tempstring);
            printf("Saved rollers in %s \n",tempstring.c_str());


            break;

        case cm_show:
            //printf("show a die or show all die\n");
            if(myargs.arg1<=0)
                {
                    //printf("no argument supplied \n");
                    mylist->showdice(false);
                }
                else{
                        //printf("got argument %s -> %i \n", tempstring.c_str(),arg1);
                        mylist->showdice(myargs.arg1);
                };
            break;

        case cm_sumroll: arg5=true;
        case cm_roll:
            //printf("roll a die\n");
            savedcommand.assign(input);
            //printf("saving input: %s -->%s \n",input.c_str(),savedcommand.c_str());
            if(myargs.arg2<0){myargs.arg2=1;}; //if it wasn't found, roll it 1 times
            //printf("%i, %c\n",myargs.arg1,myargs.args[1]);
            if((myargs.arg1==-6)&&(isdigit(myargs.args[1])==false)){printf("incorrect dX format\n");break;};
            if(myargs.args.length()!=0)//args was initialized
            {
                mylist->rolldie(myargs.args,myargs.arg2,myargs.arg3,myargs.arg4,arg5);
            }
            else if(myargs.arg1>0)//arg1 was found and it's not the -6 mask
            {
                mylist->rolldie(myargs.arg1,myargs.arg2,myargs.arg3,myargs.arg4);
            };


            break;

        case cm_uncontested:
            savedcommand.assign(input);
            if((myargs.arg1<=0)||(myargs.arg3<0)){printf("invalid arguments\n");break;};
            mylist->uncontested(myargs.arg1,myargs.arg2,myargs.arg3);
            break;

        case cm_contested:
            savedcommand.assign(input);
            if((myargs.arg1<=0)||(myargs.arg3<=0)){printf("invalid arguments\n");break;};
            mylist->contested(myargs.arg1,myargs.arg2,myargs.arg3,myargs.arg4);
            break;

        case cm_clash:
            savedcommand.assign(input);
            if((myargs.arg1<=0)||(myargs.arg3<=0)){printf("invalid arguments\n");break;};
            mylist->clash(myargs.arg1,myargs.arg2,myargs.arg3,myargs.arg4);
            break;

        case cm_reset:
            resetlist();
            resetRlist();
            printf("List reset \n");
            break;

        case cm_exit:
            exitflag=true;
            printf("Program terminated \n");
            break;

        case cm_previous:
            //printf("repeat command\n");
            repeatflag=true;
            break;

        case cm_help:
            printf("0) press enter to enter again the previous roll command\n\n");
            printf("1a) (s)how: show the dice of the list above \n");
            printf("1b) (s)how X: show the list of faces of die X \n");
            printf("1c) (crs)how: show the custom rollers of the list above \n");
            printf("1d) (crs)how X: show the contents of custom roller X \n");
            printf("1e) stoggle: toggle the visibility of the dice list\n\n");

            printf("2a) (m)ove X Y: move the Xth die to the Yth positon\n");
            printf("2a) (crm)ove X Y: move the Xth custom roller to the Yth positon\n\n");

            printf("3) save: save your current dicelist to a masterlist file, and the custom roller list to a savefile with similar name\n");
            printf("4) (ml)oad: delete current lists and load masterlist and its associated custom roller savefile\n");
            printf("5) (ol)oad: delete current list and load only dice from selected file; also purgers incompatible rollers\n");
            printf("6a) (l)oad: appends dice from selected file in dice directory to the current list\n");
            printf("6b) (crl)oad: appends COMPATIBLE rollers from selected file in rollers directory to the current list\n\n");
            printf("7a) (del)ete X: delete the Xth die from the list\n");
            printf("7b) (crdel)ete X: delete the Xth custom roller from the list\n");
            printf("8) (res)et: delete the current list and all of its rollers\n\n");

            printf("9) (r)oll X Y A B: roll Xth die Y times, with B boons(or banes if negative) and bonus A\n");
            printf("9b) (r)oll dX Y A B: roll a dX Y times, with B boons/banes and bonus A\n");
            printf("9c) (sr)oll dX Y A B: like roll dX, but also gives the sum\n");
            printf("9d) (cr)oll X Y: roll custom roller X with seed Y\n");
            printf("9e) u(ncon)tested A MA DC: check best of A d6, adds MA subs DC; result is Degree of Success (also accepts 'uncon')\n");
            printf("9f) (con)tested A MA D MD: check best of A d6 (+MA) vs best of D d6 (+MD); difference is Degree of Success\n");
            printf("9e) (cl)ash A MA D MD: rolls each of A d6 (+MA) vs each of D d6 (+MD) (unpaired count as 0), giving each Degree of Success\n\n");
            printf("9f) pix x: generate x random pictures from the res/picture folder");

            printf("10) e(x)it: exit program\n");
            printf("11) (h)elp: show this help message\n");
            printf("12) (p)revious, or [enter]: repeat previous roll (9 through 9e)\n");
            //printf("13) crmake: build a custom roller\n");
            break;

        case cm_crmove:
            //printf("got argument %s -> %i , %i \n", tempstring.c_str(),arg1, arg2);
            if((myargs.arg1<=0)||(myargs.arg2<=0)){printf("usage is crmove X Y, moves Xth roller to become the Yth\n"); break;};
            if((myargs.arg1>myrlist->getrollernumber())||(myargs.arg2>myrlist->getrollernumber())){printf("indices out of bounds\n");break;};
            myrlist->putroller(myargs.arg1,myargs.arg2);
            printf("Moved roller %i to position %i\n", myargs.arg1,myargs.arg2);
            break;

        case cm_crshow:
            //printf("show a die or show all die\n");
            if(myargs.arg1<=0)
                {
                    //printf("no argument supplied \n");
                    myrlist->showrollers(false);
                }
                else{
                        //printf("got argument %s -> %i \n", tempstring.c_str(),arg1);
                        myrlist->showrollers(myargs.arg1);
                };
            break;

        case cm_crload:
            //printf("load compatible rollers from list \n");
            if(mylist->getdicenumber()==0){printf("Die list is empty, cannot load any roller\n");break;};
            printf("Enter number of rollerlist to append, leave empty to go back\n");
            tempstring=fileselect("res/rollers/");

            if(tempstring.compare("NOFILE")!=0)
            {
                myrlist->append(tempstring);
                fix();
                //printf("\nbefore CC\n");
                //myrlist->showrollers(false);

                compatibility_check();
            };
            break;


        case cm_crdelete:
            //printf("delete a roller \n");
            if(myargs.arg1<=0){printf("Delete is used 'crdelete X', where X is the number of the roller \n");break;};
            //printf("got argument %s -> %i \n", tempstring.c_str(),arg1);
            myrlist->delroller(myargs.arg1);
            printf("Done. \n");
            break;


        case cm_crroll:
            savedcommand.assign(input);
            if((myargs.arg1<=0)||(myargs.arg1>myrlist->getrollernumber())){printf("Index out of bounds\n");break;};
            if(myargs.arg2<0){myargs.arg2=0;};
            //printf("arg1 %i; arg2 %i\n",myargs.arg1,myargs.arg2);
            //printf("entries detected %i\n\n",myrlist->getroller(myargs.arg1)->roller->getentriestotal());
            for(i=1;i<=myrlist->getroller(myargs.arg1)->roller->getentriestotal();i++)
            {
                //printf("ENTRY %i:",i);
                //printf("'%s'\n",myrlist->getroller(myargs.arg1)->roller->getentry(i).c_str());
                //printf("SEEDED: '%s'",seedR(myrlist->getroller(myargs.arg1)->roller->getentry(i),myargs.arg2).c_str());
                //printf(" ISROLL?:%i \n",myrlist->getroller(myargs.arg1)->roller->isroll(seedR(myrlist->getroller(myargs.arg1)->roller->getentry(i),myargs.arg2)));
                if(myrlist->getroller(myargs.arg1)->roller->isroll(seedR(myrlist->getroller(myargs.arg1)->roller->getentry(i),myargs.arg2))==false)
                {
                    //printf("not roll -> ");
                    cprintf(seedR(myrlist->getroller(myargs.arg1)->roller->getentry(i),myargs.arg2));
                    //printf("\n");
                }
                else
                {
                    //printf("isroll \n");

                    crargs=getargs(seedR(myrlist->getroller(myargs.arg1)->roller->getentry(i),myargs.arg2));
                    switch(CMCV(crargs.command))
                    {
                        case cm_sumroll: arg5=true;
                        case cm_roll: //it's either a dX or a dicename (you can't put a number as dicename in custom rolls)
                            //printf("crarggs: command:'%s' 1:%i 2:%i 3:%i 4:%i sum:%i S:'%s'\n",crargs.command.c_str(),crargs.arg1,crargs.arg2,crargs.arg3,crargs.arg4,arg5,crargs.args.c_str());
                            if(crargs.arg1==0) //it's a dx roll
                            {
                               if(arg5==false) //then roll it one at the time
                               {
                                   for(j=1;j<=crargs.arg2;j++)
                                    {
                                        if(j!=1){printf(" ");};
                                        printf("%i", mylist->rolldie(crargs.args,1,crargs.arg3,crargs.arg4,false,true));
                                    };

                               }else //roll and give the sum
                               {
                                   printf("%i", mylist->rolldie(crargs.args,crargs.arg2,crargs.arg3,crargs.arg4,true,true));
                               };

                            }
                            else if(crargs.arg1==-6) //it's a dicename, roll it one at the time
                            {
                                for(j=1;j<=crargs.arg2;j++)
                                {
                                    if(j!=1){printf(" ");};
                                    printf("%s", mylist->rolldie(mylist->getdieposition(crargs.args),1,crargs.arg3,crargs.arg4,true).c_str());
                                };

                            };

                        break;

                        case cm_contested:
                                myrlist->contested(crargs.arg1,crargs.arg2,crargs.arg3,crargs.arg4);
                                break;

                        case cm_clash:
                                myrlist->clash(crargs.arg1,crargs.arg2,crargs.arg3,crargs.arg4);
                                break;

                        case cm_uncontested:
                                myrlist->uncontested(crargs.arg1,crargs.arg2,crargs.arg3);
                                break;
                        default: printf("ERROR: roll command unrecognized or wrong format for 'roll' command\n");

                    };

                };
                //printf("\n\n");
            };

            break;


        /*
        case cm_crmake: printf("build a custom roller, then edit it \n");break;
        case cm_credit:
            if(myrlist->getrollernumber()<=0){printf("no rollers to edit\n");break;};
            printf("Select the roller to edit \n");
            myrlist->showrollers(false);
            printf("\n>");
            i=0;
            scanf("%i",&(i));
            getchar();


            while (CMCV(myargs.command)!=cm_exit)
            {
                clearscreen();
                printf("\n");myrlist->showrollers(i);
                printf("\n Available commands: \n 1)[ei]nsert X\n 2)[ed]elete X\n 3)[em]ove X Y\n 4)[er]eplace X\n 5)e[x]it\n");
                printf("For insert and replace you will be prompted to type a line afterwards\n ");
                printf("\n>");

                tempstring.clear();
                getline(cin, tempstring);
                myargs=getargs(tempstring);
                switch(CMCV(myargs.command))
                {
                    case cm_editdelete:
                        if(myrlist->getroller(i)->roller->getentriestotal()<=0){printf("no entries to delete\n");break;};
                        if((myargs.arg1<=0)||(myargs.arg1>myrlist->getroller(i)->roller->getentriestotal())) {printf("invalid argument\n");break;};
                        printf("LINE %i:'%s'\n",myargs.arg1,myrlist->getroller(i)->roller->getentry((int)myargs.arg1).c_str());
                        printf("Delete line %i? (y/n)\n",myargs.arg1);
                        printf(">");
                        tempstring.clear();
                        getline(cin,tempstring);
                        if(tempstring[0]=='y'){myrlist->getroller(i)->roller->delentry(myargs.arg1);};
                        break;

                    case cm_editinsert:
                        if((myargs.arg1<=0)||(myargs.arg1>myrlist->getroller(i)->roller->getentriestotal()+1)){printf("index out of bounds\n");break;};
                        printf("LINE %i:'%s'\n",myargs.arg1,myrlist->getroller(i)->roller->getentry((int)myargs.arg1).c_str());
                        printf("Enter the line that will go after this\n");
                        printf(">");
                        tempstring.clear();
                        getline(cin,tempstring);
                        break;

                    case cm_editmove:printf("move a line\n");break;
                    case cm_editreplace:printf("replace a line\n");break;
                    case cm_exit:printf("exit subroutine\n");break;
                    default: printf("command not recognized\n");break;
                };
                getchar();
            };
            break;
        */
                    case cm_pix:
                        //if(myargs.arg1<=0){myargs.arg1=3;}; //default roll 3 pictures
                        //if(myargs.arg1>10){myargs.arg1=10;}; //max 10 pictures
                        if(sdlLoaded==false){printf("pixroll disabled because SDL failed to initialize\n");return;};
                        savedcommand.assign(input);
                        //printf("PIXROLL\n");
                        pixroll(myargs.arg1);
                        break;

                    case cm_stoggle:
                        printf("toggled dice visibility");
                        if(hideflag==true){hideflag=false;}else{hideflag=true;};
                        break;



        default: printf("Command not recognized\n");break;

    };

};

void programstate::loop()
{
    savedcommand.clear();
    exitflag=false;
    do{

        if (repeatflag==true)
        {
            printf("%s\n",savedcommand.c_str());
            parser(savedcommand);
            repeatflag=false;
        }else
        {
            clearscreen();
            mystring.clear();
            if(hideflag==false){mylist->showdice(false);};
            myrlist->showrollers(false);
            printf("\n>");
            getline(cin,mystring);
            parser(mystring);

        };
        if(repeatflag==false){getchar();};

    }while (exitflag==false);

};

std::string programstate:: index_to_dicename (std::string content)
{
    arglist myargs=getargs(content);
    std::string output;
    onedie* tempp=mylist->getlist();
    output.clear();

    if((myargs.arg1<=0)||(myargs.arg1>mylist->getdicenumber()))
    //{printf("arg1(%i) is not a dice index number or is out of bounds(%i)\n",myargs.arg1,mylist->getdicenumber()); return output;};

    //printf("%i) current name: %s\n",myargs.arg1,tempp->die->getname().c_str());
    while(myargs.arg1>1)
    {
        tempp=tempp->next;
        myargs.arg1-=1;
        //printf("%i) current name: %s\n",myargs.arg1,tempp->die->getname().c_str());
    };
    output.assign(tempp->die->getname());

    //printf("got dicename '%s'\n",output.c_str());
    return output;

};


void programstate::rollerfixer(oneroller* target)
{
    arglist myargs=getargs("");
    std::string tempstring;
    tempstring.clear();
    char tempchar[4];
    int i=0;
    for(i=1;i<=target->roller->getentriestotal();i++)
    {
      // printf("%i) '%s' isroll? %i\n\n",i,target->roller->getentry(i).c_str(), target->roller->isroll(target->roller->getentry(i)));


        if(target->roller->isroll(target->roller->getentry(i))==true)
           {
               //printf("rebuilding entry\n");
               //get args
               myargs=getargs(target->roller->getentry(i));
              // printf("arg1:%i\n",myargs.arg1);
               if((CMCV(myargs.command)==cm_roll)&&(myargs.arg1>0)) //otherwise it's already converted OR it's a dx; also means args is empty;
               {
                   //NOTE: This is to be used after creating a custom roller; indices will be those of dice already in the list.
                   //out-of-list indices will get corrupted

                   if(mylist->getdie(myargs.arg1)!=NULL)
                   {
                       myargs.args.assign(mylist->getdie(myargs.arg1)->die->getname());
                       //printf("\n\n LINE:%i GOT NAME OF DICE %i:'%s'\n", i, myargs.arg1, myargs.args.c_str());

                       //tempstring will be: roll, dicename, arg2, arg3, arg4
                       tempstring.append(myargs.command);
                       tempstring.append(" ");

                       tempstring.append(sR(myargs.args));
                       tempstring.append(" ");

                       snprintf(tempchar, 4, "%i",myargs.arg2);
                       tempstring.append(tempchar);
                       tempstring.append(" ");

                       snprintf(tempchar, 4, "%i",myargs.arg3);
                       tempstring.append(tempchar);
                       tempstring.append(" ");

                       snprintf(tempchar, 4, "%i",myargs.arg4);
                       tempstring.append(tempchar);

                       //printf("OLD:'%s' NEW:'%s'\n", target->roller->getentry(i).c_str(), tempstring.c_str());
                       target->roller->setentry(i, tempstring);
                       //printf("CHANGED: '%s'\n\n",target->roller->getentry(i).c_str());
                   }
                   else
                   {
                       //printf("FIXROLLER: invalid die number\n");
                       //tempstring.assign(target->roller->getentry(i));
                       //tempstring.insert(4,"_");
                       //target->roller->setentry(i, tempstring);
                   };

               };

               tempstring.clear();
               tempchar[0]=tempchar[1]=tempchar[2]=tempchar[3]='\0';
               myargs=getargs("");
             };
    };
};


void programstate::fix()
{
    oneroller* tempp=NULL;
    int i=0;
    for (i=1;i<=myrlist->getrollernumber();i++)
    {
        tempp=myrlist->getroller(i);
        rollerfixer(tempp);
    };
};

void programstate::compatibility_check()
{
    onedie* tempp=NULL;
    int rollerindex=1;
    int entryindex=1;
    arglist myargs;
    bool eraseflag=false;

    if(myrlist->getrollernumber()==0){return;};

    tempp=myrlist->getroller(1);

    do //over all rollers
    {
        //printf("\nCC: examining roller:'%s'\n",tempp->roller->getname().c_str());

        while((entryindex<=tempp->roller->getentriestotal())&&(eraseflag==false)) //over all entries for this roller; interrupts as soon as eraseflag is true
        {
            myargs=getargs(seedR(tempp->roller->getentry(entryindex),1));//I'm putting a seed over the # to perform t
            //if the line is a roll, but the die to roll wasn't found;
            //printf("checking args:'%s'\n",myargs.args.c_str());
                //if isroll==true, and it's a dicename (arg1==-6) but the dice wasn't found (getdiepos==0)
            if((tempp->roller->isroll(tempp->roller->getentry(entryindex))==true)&&((myargs.arg1==-6)&&(mylist->getdieposition(myargs.args)==0)))
            {
                //printf("die not found\n");
                eraseflag=true;
            };
            entryindex++;

        };

        if(eraseflag==true)
        {
            printf("roller %i:'%s' refers to dice '%s' which is not present, and will be deleted\n",
                   rollerindex,tempp->roller->getname().c_str(), myargs.args.c_str());
            myrlist->delroller(rollerindex);
            printf("roller deleted\n");
            eraseflag=false;
        }
        else
        {
            rollerindex++;
        };

        tempp=myrlist->getroller(rollerindex);
        entryindex=1;
    }while(tempp!=NULL);
};

///////////////////////////////////////////////////

