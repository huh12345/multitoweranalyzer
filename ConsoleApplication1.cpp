// ConsoleApplication1.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#define temoti 2 //相手の手持ち マルチは２体


unsigned long seed = 0; //日替わり
int round = 1; //現在何週目か ex)８～１４連勝は２週目
int count = 1; //本日の何回目の挑戦か 初めてのときは1
int kaisu = 0;

class trainer{
public:
	std::string name;
	std::vector<int> pokemon_data;
};

class pokemon{
public:
	std::string name;
	std::string nature;
	std::string item;
	std::string move; // 技
	std::string ev; // 努力値
};

class pokedex{
public:
	std::string name;
	int hp;
	int attack;
	int block;
	int contact;
	int defense;
	int speed;
	std::string ability1;
	std::string ability2;
	std::string gender;
};


std::string nature[25] = {"がんばりや","さみしがり","ゆうかん","いじっぱり","やんちゃ",
						  "ずぶとい",  "すなお",    "のんき",  "わんぱく",  "のうてんき",
						  "おくびょう","せっかち",  "まじめ",  "ようき",    "むじゃき",
						  "ひかえめ",  "おっとり",  "れいせい","てれや",    "うっかりや",
						  "おだやか",  "おとなしい","なまいき","しんちょう","きまぐれ"};

int trainer_table_min[8] = {0,  80,  100, 120, 140, 160, 180, 200}; //１～６戦目
int trainer_table_max[8] = {99, 119, 139, 159, 179, 199, 219, 299};

int sptrainer_table_min[8] = {100, 120, 140, 160, 180, 200, 220, 200}; //７戦目
int sptrainer_table_max[8] = {119, 139, 159, 179, 199, 219, 239, 299};

unsigned long mul(unsigned long a,unsigned long b){
	unsigned long a1 = a >> 16; //上位ビット
	unsigned long a2 = a & 0xffff; //下位ビット
	unsigned long b1 = b >> 16;
	unsigned long b2 = b & 0xffff;
	return ((a1 * b2 + a2 * b1) << 16) + a2 * b2;
}


unsigned long syouhi(){ //バトルタワー
	seed = mul(seed,0x02e90edd) + 1;
	kaisu++;
	return seed / 0xffff % 0xffff;
}

void syouhi_higawari(){ //日替わりくじ
	seed = mul(seed,0x6c078965) + 1;
}

void syouhi_syoki(){ //日替わりから初期消費
	seed = mul(seed,0x5D588B65) + 1;
}

void syouhia(){ //通常
	seed = mul(seed,0x41c64e6d) + 0x6073;
}

unsigned long long calcid(){ //tidとpid
	unsigned long low = syouhi();
	unsigned long high = syouhi();
	return (high << 16 | low);
}

int _tmain(int argc, _TCHAR* argv[])
{
	std::ifstream trainerdata("trainer.txt");
	std::ifstream usedata("tower-trainers-table.txt");
	std::ifstream pokemondata("factory_data.txt");
	std::ifstream pokedexdata("pokedex.csv");

	std::vector<trainer> trainerList;
	std::vector<pokemon> pokemonList;
	std::vector<pokedex> pokedexList;

	trainer tmp_tra;
	pokemon tmp_poke;
	pokedex tmp_dex;

	int vstrainer[14][1 + temoti]; //実際に戦うトレーナーと連れているポケモンの番号
	int partner[5][temoti]; //パートナーの連れているポケモンの番号 モミ,ミル,ゲン,マイ,バクの順
	std::string player[temoti]; //プレイヤーが連れているポケモン
	std::string item[temoti]; //プレイヤーが連れているポケモンに持たせているアイテム
	std::vector<std::string> abilitydata; // 現在参照しているトレーナーのポケモンの特性
	char *tok; //strtok_s

	std::string str;
	char* str_pokedata;

	int n; //手持ち検査用
	bool flag; //手持ち検査用
	int pokenum;

	unsigned long long tid,pid;

	trainerList.clear();
	pokemonList.clear();
	pokedexList.clear();
	abilitydata.clear();

	//各種ファイル読み込み
	for(int i=0;i<305;i++){
		std::getline(trainerdata,str);
		tmp_tra.name = str;
		tmp_tra.pokemon_data.clear();
		trainerList.push_back(tmp_tra);
	}

	for(int i=0;i<305;i++){
		std::getline(usedata,str);
		str_pokedata = strtok_s((char*)str.c_str(),",",&tok);
		
		trainerList.at(i).pokemon_data.push_back((int)strtod(str_pokedata,NULL));
		while (str_pokedata!=NULL){
			str_pokedata = strtok_s(NULL,",",&tok);
			if (str_pokedata!=NULL){
				trainerList.at(i).pokemon_data.push_back((int)strtod(str_pokedata,NULL));
			}
		}
	}

	for(int i=0;i<950;i++){
		std::getline(pokemondata,str);
		
		str_pokedata = strtok_s((char*)str.c_str()," ",&tok);
		tmp_poke.name = str_pokedata;
		str_pokedata = strtok_s(NULL," ",&tok);
		tmp_poke.nature = str_pokedata;
		str_pokedata = strtok_s(NULL," ",&tok);
		tmp_poke.item = str_pokedata;
		str_pokedata = strtok_s(NULL," ",&tok);
		tmp_poke.move = str_pokedata;
		str_pokedata = strtok_s(NULL," ",&tok);
		tmp_poke.ev = str_pokedata;

		pokemonList.push_back(tmp_poke);
	}

	for(int i=0;i<493;i++){
		std::getline(pokedexdata,str);

		str_pokedata = strtok_s((char*)str.c_str(),",",&tok);
		tmp_dex.name = str_pokedata;

		str_pokedata = strtok_s(NULL,",",&tok);
		tmp_dex.hp = (int)strtod(str_pokedata,NULL);
		str_pokedata = strtok_s(NULL,",",&tok);
		tmp_dex.attack = (int)strtod(str_pokedata,NULL);
		str_pokedata = strtok_s(NULL,",",&tok);
		tmp_dex.block = (int)strtod(str_pokedata,NULL);
		str_pokedata = strtok_s(NULL,",",&tok);
		tmp_dex.contact = (int)strtod(str_pokedata,NULL);
		str_pokedata = strtok_s(NULL,",",&tok);
		tmp_dex.defense = (int)strtod(str_pokedata,NULL);
		str_pokedata = strtok_s(NULL,",",&tok);
		tmp_dex.speed = (int)strtod(str_pokedata,NULL);

		str_pokedata = strtok_s(NULL,",",&tok);
		tmp_dex.ability1 = str_pokedata;
		str_pokedata = strtok_s(NULL,",",&tok);
		tmp_dex.ability2 = str_pokedata;
		if(tmp_dex.ability2 == "1:1" || tmp_dex.ability2 == "1:3" || tmp_dex.ability2 == "1:7" || tmp_dex.ability2 == "3:1" || 
		   tmp_dex.ability2 == "♂のみ" || tmp_dex.ability2 == "♀のみ" || tmp_dex.ability2 == "ふめい"){
			tmp_dex.gender = tmp_dex.ability2;
			tmp_dex.ability2 = tmp_dex.ability1;
		}
		else{
			str_pokedata = strtok_s(NULL,",",&tok);
			tmp_dex.gender = str_pokedata;
		}

		pokedexList.push_back(tmp_dex);
	}

	trainerdata.close();
	usedata.close();
	pokemondata.close();
	pokedexdata.close();

	std::cout << "日替わりseed :";
	std::cin >> str;
	seed = strtoul((char*)str.c_str(),NULL,0);
	
	std::cout << "現在何週目か ex)８～１４連勝は２週目 :";
	std::cin >> str;
	round = (int)strtol((char*)str.c_str(),NULL,0);

	std::cout << "本日の何回目の挑戦か 初めてのときは1 :";
	std::cin >> str;
	count = (int)strtol((char*)str.c_str(),NULL,0);

	std::cout << "使用ポケモン1 :";
	std::cin >> player[0];

	std::cout << "使用ポケモン2 :";
	std::cin >> player[1];

	std::cout << "使用アイテム1 :";
	std::cin >> item[0];

	std::cout << "使用アイテム2 :";
	std::cin >> item[1];

	//初期消費
	for(int i=0;i<count;i++){
		syouhi_syoki();
	}
	for(int i=0;i<((round - 1) * 24 + 1);i++) syouhi();

	if (round > 8) round = 8;
	else if (round < 1) round = 1;

	//敵トレーナーの決定
	for(int i=0;i<12;i++){
		n = 0;
		vstrainer[i][0] = ((int)syouhi() % (trainer_table_max[round-1] - trainer_table_min[round-1] + 1)) + trainer_table_min[round-1];
		while(n < i){
			if(vstrainer[i][0] == vstrainer[n][0]){
				vstrainer[i][0] = ((int)syouhi() % (trainer_table_max[round-1] - trainer_table_min[round-1] + 1)) + trainer_table_min[round-1];
			}
			else n++;
		}
	}
	for(int i=12;i<14;i++){
		n = 0;
		vstrainer[i][0] = ((int)syouhi() % (sptrainer_table_max[round-1] - sptrainer_table_min[round-1] + 1)) + sptrainer_table_min[round-1];
		while(n < i){
			if(vstrainer[i][0] == vstrainer[n][0]){
				vstrainer[i][0] = ((int)syouhi() % (sptrainer_table_max[round-1] - sptrainer_table_min[round-1] + 1)) + sptrainer_table_min[round-1];
			}
			else n++;
		}
	}

	// パートナーのポケモン決定
	std::cout << "パートナー" << std::endl;
	for(int i=0;i<5;i++){
		std::cout << trainerList.at(i+300).name << std::endl;
		//ポケモンの決定
		n = 0;
		while(n < temoti){
			pokenum = (int)syouhi() % trainerList.at(i+300).pokemon_data.size();
			pokenum = trainerList.at(i+300).pokemon_data.at(pokenum);
			flag = true;
			for(int j=0;j<n;j++){
				if(pokemonList.at(pokenum-1).name == pokemonList.at(partner[i][j]).name) flag = false;
				if(pokemonList.at(pokenum-1).item == pokemonList.at(partner[i][j]).item) flag = false;
			}
			for(int j=0;j<temoti;j++){
				if(pokemonList.at(pokenum-1).name == player[j]) flag = false;
				if(pokemonList.at(pokenum-1).item == item[j]) flag = false;
			}
			if(flag == true){
				partner[i][n] = pokenum - 1;
				n++;
			}
		}
		// tid
		tid = calcid();

		// 各ポケモンのpidおよび特性
		for(int j=0;j<temoti;j++){
			pid = calcid();
			while((nature[pid%25] != pokemonList.at(partner[i][j]).nature) ||
				  (((tid >> 16) ^ (tid & 0xffff) ^ (pid >> 16) ^ (pid & 0xffff)) < 8)){ // 性格一致して色違いでなくなるまで
 				pid = calcid();
			}
			n = 0;
			while(pokemonList.at(partner[i][j]).name != pokedexList.at(n).name) n++;
			if(pid%2==0) abilitydata.push_back(pokedexList.at(n).ability1);
			else  abilitydata.push_back(pokedexList.at(n).ability2);
		}
		for(int j=0;j<temoti;j++){
			std::cout << pokemonList.at(partner[i][j]).name << " " << pokemonList.at(partner[i][j]).item << " " << abilitydata.at(j) << std::endl;
			std::cout << pokemonList.at(partner[i][j]).move << " " << pokemonList.at(partner[i][j]).nature << " " << pokemonList.at(partner[i][j]).ev << std::endl;
		}
		abilitydata.clear();
		std::cout << std::endl;
	}

	std::cout << std::endl;
	//敵トレーナーのポケモン決定
	std::cout << "敵トレーナー" << std::endl;
	for(int i=0;i<14;i++){
		std::cout << (i/2)+1 << " " << trainerList.at(vstrainer[i][0]).name << std::endl;
		//ポケモンの決定
		n = 0;
		while(n < temoti){
			pokenum = (int)syouhi() % trainerList.at(vstrainer[i][0]).pokemon_data.size();
			pokenum = trainerList.at(vstrainer[i][0]).pokemon_data.at(pokenum);
			flag = true;
			for(int j=0;j<n;j++){
				if(pokemonList.at(pokenum-1).name == pokemonList.at(vstrainer[i][1+j]).name) flag = false;
				if(pokemonList.at(pokenum-1).item == pokemonList.at(vstrainer[i][1+j]).item) flag = false;
			}
			if(i%2 == 1){
				for(int j=0;j<temoti;j++){
					if(pokemonList.at(pokenum-1).name == pokemonList.at(vstrainer[i-1][1+j]).name) flag = false;
					if(pokemonList.at(pokenum-1).item == pokemonList.at(vstrainer[i-1][1+j]).item) flag = false;
				}
			}
			if(flag == true){
				vstrainer[i][1+n] = pokenum - 1;
				n++;
			}
		}
		// tid
		tid = calcid();

		// 各ポケモンのpid
		for(int j=0;j<temoti;j++){
			pid = calcid();
			while((nature[pid%25] != pokemonList.at(vstrainer[i][1+j]).nature) ||
				  (((tid >> 16) ^ (tid & 0xffff) ^ (pid >> 16) ^ (pid & 0xffff)) < 8)){ // 性格一致して色違いでなくなるまで
 				pid = calcid();
			}
			n = 0;
			while(pokemonList.at(vstrainer[i][1+j]).name != pokedexList.at(n).name) n++;
			if(pid%2==0) abilitydata.push_back(pokedexList.at(n).ability1);
			else  abilitydata.push_back(pokedexList.at(n).ability2);
		}

		for(int j=0;j<temoti;j++){
			std::cout << pokemonList.at(vstrainer[i][1+j]).name << " " << pokemonList.at(vstrainer[i][1+j]).item << " " << abilitydata.at(j) << std::endl;
			std::cout << pokemonList.at(vstrainer[i][1+j]).move << " " << pokemonList.at(vstrainer[i][1+j]).nature << " " << pokemonList.at(vstrainer[i][1+j]).ev << std::endl;
		}
		abilitydata.clear();
		if(i%2==1) syouhi();
		std::cout << std::endl;
	}



	std::string abc;
	std::cin >> abc;
	return 0;
}

