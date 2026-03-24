#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cmath>

using namespace std;

int roll(int s){ return rand()%s+1; }

int rollStat(){
    int r[4];
    for(int i=0;i<4;i++) r[i]=roll(6);
    sort(r,r+4);
    return r[1]+r[2]+r[3];
}

int mod(int s){ return floor((s-10)/2.0); }

string pick(const vector<string>& v){
    return v[rand()%v.size()];
}

vector<string> maleFirst = {"Aelar","Thorin","Darius","Kael","Ronan","Lucan","Draven"};
vector<string> femaleFirst = {"Seraphine","Lyra","Mira","Nyssa","Selene","Aria","Elara"};
vector<string> lastNames = {"Blackwood","Ironfist","Stormborn","Nightveil","Dawnbringer","Grimward"};

vector<string> titles = {
    "The Black Blade","High Inquisitor","The Silent Hand",
    "Warden of Ash","The Crimson Wolf"
};

struct Role {
    string name;
    string cls;
    string primary;
    string secondary;
    vector<string> skills;
};

vector<Role> roles = {
    {"Guard Captain","Fighter","STR","CON",{"Athletics","Intimidation","Perception"}},
    {"Assassin","Rogue","DEX","INT",{"Stealth","Deception"}},
    {"Priest","Cleric","WIS","CON",{"Religion","Insight"}},
    {"Mage","Wizard","INT","DEX",{"Arcana","Investigation"}},
    {"Scout","Ranger","DEX","WIS",{"Survival","Perception","Stealth"}},
    {"Cult Leader","Warlock","CHA","CON",{"Deception","Religion"}},
    {"Knight","Paladin","STR","CHA",{"Athletics","Persuasion"}},
    {"Performer","Bard","CHA","DEX",{"Performance","Persuasion"}}
};

struct NPC {
    string name;
    string title;
    string gender;
    Role role;
    string type;
    int STR=0,DEX=0,CON=0,INT=0,WIS=0,CHA=0;
    int HP=0, AC=0, atk=0, dmg=0;
    string weapon;
    vector<string> skills;
    string personality, goal, flaw;
    string reputation;
};

void assignStats(NPC& n){
    vector<int> s;
    for(int i=0;i<6;i++) s.push_back(rollStat());
    sort(s.begin(),s.end(),greater<int>());

    vector<int*> stats={&n.STR,&n.DEX,&n.CON,&n.INT,&n.WIS,&n.CHA};

    if(n.role.primary=="STR") n.STR=s[0];
    if(n.role.primary=="DEX") n.DEX=s[0];
    if(n.role.primary=="CON") n.CON=s[0];
    if(n.role.primary=="INT") n.INT=s[0];
    if(n.role.primary=="WIS") n.WIS=s[0];
    if(n.role.primary=="CHA") n.CHA=s[0];

    if(n.role.secondary=="STR") n.STR=s[1];
    if(n.role.secondary=="DEX") n.DEX=s[1];
    if(n.role.secondary=="CON") n.CON=s[1];
    if(n.role.secondary=="INT") n.INT=s[1];
    if(n.role.secondary=="WIS") n.WIS=s[1];
    if(n.role.secondary=="CHA") n.CHA=s[1];

    int idx=2;
    for(auto st:stats){
        if(*st==0) *st=s[idx++];
    }
}

void scale(NPC& n){
    int base=10+mod(n.CON);

    if(n.type=="Standard"){
        n.HP=base+10; n.atk=3; n.dmg=6;
    }
    else if(n.type=="Elite"){
        n.HP=base+25; n.atk=5; n.dmg=8;
    }
    else{
        n.HP=base+60; n.atk=7; n.dmg=12;
    }

    n.AC=10+mod(n.DEX)+2;
}

NPC createNPC(string type){
    NPC n;
    n.type=type;

    if(rand()%2==0){
        n.gender="Male";
        n.name=pick(maleFirst);
    } else{
        n.gender="Female";
        n.name=pick(femaleFirst);
    }

    n.name += " " + pick(lastNames);
    n.role=roles[rand()%roles.size()];
    assignStats(n);
    scale(n);

    if(type=="Boss") n.title=pick(titles);

    n.weapon="Weapon";
    n.skills=n.role.skills;

    vector<string> p={"Cold","Charismatic","Disciplined","Brutal"};
    vector<string> g={"Power","Revenge","Protection","Control"};
    vector<string> f={"Arrogant","Greedy","Paranoid","Reckless"};
    vector<string> r={"Feared","Respected","Unknown","Infamous"};

    n.personality=pick(p);
    n.goal=pick(g);
    n.flaw=pick(f);
    n.reputation=pick(r);

    return n;
}

void printNPC(NPC& n){
    cout<<"\n====================================\n";
    cout<<n.name;
    if(n.type=="Boss") cout<<" - "<<n.title;
    cout<<"\n("<<n.role.name<<" | "<<n.type<<")\n";
    cout<<"====================================\n";
    cout<<"HP: "<<n.HP<<" | AC: "<<n.AC<<"\n";
    cout<<"\nSTR "<<n.STR<<" ("<<mod(n.STR)<<") "
        <<"DEX "<<n.DEX<<" ("<<mod(n.DEX)<<") "
        <<"CON "<<n.CON<<" ("<<mod(n.CON)<<")\n";
    cout<<"INT "<<n.INT<<" ("<<mod(n.INT)<<") "
        <<"WIS "<<n.WIS<<" ("<<mod(n.WIS)<<") "
        <<"CHA "<<n.CHA<<" ("<<mod(n.CHA)<<")\n";
    cout<<"\nAttack: +"<<n.atk<<" | Damage: "<<n.dmg<<"\n";
    if(n.type=="Boss"){
        cout<<"Special: Multiattack + Unique Trait\n";
    }
    cout<<"\nSkills: ";
    for(auto&s:n.skills) cout<<s<<" ";
    cout<<"\n";
    cout<<"\n--- Identity ---\n";
    cout<<"Reputation: "<<n.reputation<<"\n";
    cout<<"\n--- Roleplay ---\n";
    cout<<"Personality: "<<n.personality<<"\n";
    cout<<"Goal: "<<n.goal<<"\n";
    cout<<"Flaw: "<<n.flaw<<"\n";
    cout<<"====================================\n";
}

int main(){
    srand((unsigned)time(0));
    cout<<"Choose NPC Strength:\n1 Standard\n2 Elite\n3 Boss\n";
    int c; cin>>c;
    string t="Standard";
    if(c==2) t="Elite";
    if(c==3) t="Boss";
    NPC npc=createNPC(t);
    printNPC(npc);
    return 0;
}
