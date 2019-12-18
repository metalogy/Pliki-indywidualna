#pragma once
#include <string>
#include <vector>
//#include "Semaphore.h"
#define SIZE_OF_BLOCK 32 //w bajtach
#define SIZE_OF_DISK 1024

struct inode
{
	std::string file_name;
	std::vector<char>* first_data_block;
	std::vector<char>* second_data_block;
	std::vector<int> index_block;
	int file_size;
	int occupied_blocks_size;
	int first_data_block_index;
	int second_data_block_index;
/*
	//Synhronizacja, Jan Witczak
	Semaphore File_Mutex;
	int Read_Count;
	bool writing;
	//Synchronizacja.*/
};
class disk_drive
{
	friend class file_system;
public:
	const void print_drive();
	const void print_inode_catalogue();
	const void display_occupied_blocks(std::string file_name);
	const void display_all_information_about_all_files();
	const int search_inode(std::string file_name);
	const unsigned int search_free_block();
	const std::vector<int> search_free_blocks(int number_of_searched_blocks);
	const bool is_enough_size(std::string data);
	const bool is_enough_size(std::string data, int old_file_size);
protected:
	disk_drive();
	int number_of_blocks;
	int number_of_free_blocks;
	std::vector<std::vector<char>> disk_space;
	std::vector<inode> inode_table;
	std::vector<int> free_drive_blocks;
	inode initialize_inode(std::string file_name);
};
class file_system : public disk_drive
{
public:
	//wszystkie metody z dopiskiem v2 odnosza sie do fizycznych plikow
	file_system();
	void create_file(std::string file_name); //tworzy plik w pamieci wirtualnej
	void create_file_v2(std::string file_name); //tworzy plik "wirtualnie" jak i fizycznie
	void delete_file(std::string file_name);
	void delete_file_v2(std::string file_name); //jak createv2
	void rename_file(std::string old_file_name, std::string new_file_name);
	void rename_file_v2(std::string old_file_name, std::string new_file_name);
	void add_in_data_to_file(std::string file_name, std::string data); //dopisuje dane do pliku
	void add_in_data_to_file_v2(std::string file_name, std::string data);
	void overwrite_data_to_file(std::string file_name, std::string data); //nadpisuje dane w pliku - czysci plik i zapisuje od nowa
	void overwrite_data_to_file_v2(std::string file_name, std::string data);
	void free_up_block_for_index(int block);//czysci blok, przeznaczony do zastosowania z blokami indeksowymi
	void free_up_block_for_pointer(std::vector<char>* pointer, int pointer_block_index);//czysci blok, przeznaczony do zastosowania z blokami wskaznikowymi
	const void display_file(std::string file_name);
	const void display_file_v2(std::string file_name); //wyswietlamy zapisane w pliku
	std::string return_file_as_string(std::string file_name);
	const void display_part_of_file(std::string file_name, int start_pos, int nr_of_characters); //zwraca zadana liczbe znakow od pozycji poczatkowej
	std::string read_part_of_file(std::string file_name, int start_pos, int nr_of_characters); //zwraca zadana liczbe znakow od pozycji poczatkowej
	char return_single_char(std::string file_name, int start_pos);
	void add_in_data_to_file_overwrite(std::string file_name, std::string data); //uzywane do nadpisywania, jak v2 tylko ot wiera plik z ios::trunc
/*
	//Synhronizacja,  Jan Witczak
	void open_file(std::string file_name_);
	void close_file(std::string file_name_);

	void open_file_reading(std::string file_name_);
	void close_file_reading(std::string file_name_);
	//Synchronizacja. */
};