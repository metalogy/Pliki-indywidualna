#include "pch.h"
#include "filesystemv2.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
disk_drive::disk_drive()
{
	number_of_blocks = SIZE_OF_DISK / SIZE_OF_BLOCK; //obliczamy ilosc blokow
	number_of_free_blocks = number_of_blocks;
	std::vector<char> negative(SIZE_OF_BLOCK, -1); //blok wype?niony zerami
	for (int i = 0; i < number_of_blocks; i++)
	{
		disk_space.push_back(negative);
		free_drive_blocks.push_back(1);
	}
}
inode disk_drive::initialize_inode(std::string file_name)
{
	inode initialized;
	initialized.file_name = file_name;
	initialized.file_size = 0;
	initialized.occupied_blocks_size = 0;
	initialized.first_data_block = nullptr;
	initialized.second_data_block = nullptr;
	initialized.first_data_block_index = -1; //-1 oznacza i? bloki na nic nie wskazuja
	initialized.second_data_block_index = -1;//-1 oznacza i? bloki na nic nie wskazuja
	return initialized;
	/*
	//Synchronizacja, Jan Witczak
	initialized.File_Mutex = Semaphore(1);
	initialized.Read_Count = 0;
	initialized.writing = false;
	//Synchronizacja.
	*/
}
const int disk_drive::search_inode(std::string file_name)
{
	for (int i = 0; i < inode_table.size(); i++)
	{
		if (inode_table[i].file_name == file_name)
		{
			return i; //gdy znajdziemy plik o takiej nazwie zwracamy jego indeks
		}
	}
	return -1; //gdy nie znajdziemy inode o podanej nazwie-plik nie istniej,  zwracamy -1
}
const bool disk_drive::is_enough_size(std::string data)
{
	int needed_blocks = ceil((float)(data.length() / (float)SIZE_OF_BLOCK)); //liczba potrzebnych blokow aby zapisac plik
	if (number_of_free_blocks < needed_blocks)
	{
		return false;
	}
	else
	{
		return true;
	}
}
const bool disk_drive::is_enough_size(std::string data, int old_file_size)
{
	int needed_blocks = ceil((float)(data.length() / (float)SIZE_OF_BLOCK)) - ceil((float)(old_file_size / (float)SIZE_OF_BLOCK));
	if (number_of_free_blocks < needed_blocks)
	{
		return false;
	}
	else
	{
		return true;
	}
}

const unsigned int disk_drive::search_free_block() //zwraca indeks pierwszego napotkanego bloku mozliwego do zapisu
{
	for (int i = 0; i < free_drive_blocks.size(); i++)
	{
		if (free_drive_blocks[i] == 1)
		{
			return i;
		}
	}
}
const std::vector<int> disk_drive::search_free_blocks(int number_of_searched_blocks) //zwraca  wektor z indeksami n pierwszych blokow gotowych do zapisu
{
	std::vector<int> returning_vector;
	for (int i = 0, j = 0; (i < SIZE_OF_BLOCK) and (j < number_of_searched_blocks); i++)
	{
		if (free_drive_blocks[i] == 1)
		{
			returning_vector.push_back(i);
			j++;
		}
	}
	return returning_vector;
}
const void disk_drive::print_inode_catalogue()
{
	std::cout << std::endl << "Displaying files on disk" << std::endl;
	for (int i = 0; i < inode_table.size(); i++)
	{
		std::cout << "File name: " << inode_table[i].file_name << " File size: " << inode_table[i].file_size << std::endl;
	}
}
const void disk_drive::print_drive()
{
	for (int i = 0; i < number_of_blocks; i++)
	{
		std::cout << std::endl << "NEW BLOCK" << std::endl;
		for (int j = 0; j < SIZE_OF_BLOCK; j++)
		{
			std::cout << (int)disk_space[i][j];
		}
	}
	std::cout << std::endl;
}
const void disk_drive::display_occupied_blocks(std::string file_name) //wyswietla bloki zajete przez dany plik
{
	std::cout << std::endl << "Occupied blocks by file " << file_name << std::endl;
	int inode_index = search_inode(file_name);
	if (inode_index != -1)
	{
		if (inode_table[inode_index].first_data_block != nullptr)
		{
			std::cout << " " << inode_table[inode_index].first_data_block_index << " ";
		}
		if (inode_table[inode_index].second_data_block != nullptr)
		{
			std::cout << " " << inode_table[inode_index].second_data_block_index << " ";
		}
		if (!inode_table[inode_index].index_block.empty())
		{
			for (int i = 0, j = 0; i < inode_table[inode_index].index_block.size(); i++)
			{
				std::cout << " " << inode_table[inode_index].index_block[i] << " ";
			}
		}
		std::cout << std::endl;
	}
	else
	{
		std::cout << "File " << file_name << " doesn't exists " << std::endl;
	}

}
const void disk_drive::display_all_information_about_all_files()
{
	std::cout << std::endl << "Displaying files on disk" << std::endl;
	for (int i = 0; i < inode_table.size(); i++)
	{
		std::cout << "File name: " << inode_table[i].file_name << " File size: " << inode_table[i].file_size << " Number of occupied blocks: " << inode_table[i].occupied_blocks_size << std::endl;
		display_occupied_blocks(inode_table[i].file_name);
	}
}




file_system::file_system()
{
	disk_drive();
}
void file_system::create_file(std::string file_name)
{
	int inode_index = search_inode(file_name);
	if (inode_index != -1) //jezeli plik o danej nazwie istnieje
	{
		std::cout << "File named " << file_name << " already exists" << std::endl;
	}
	else
	{
		inode helping_inode = initialize_inode(file_name);
		disk_drive::inode_table.push_back(helping_inode);
		std::cout << "File " << file_name << " created" << std::endl;
	}
}
void file_system::create_file_v2(std::string file_name)
{
	int inode_index = search_inode(file_name);
	if (inode_index != -1) //jezeli plik o danej nazwie istnieje
	{
		std::cout << "File named " << file_name << " already exists" << std::endl;
	}
	else
	{
		inode helping_inode = initialize_inode(file_name);
		disk_drive::inode_table.push_back(helping_inode);
		std::fstream file; //tworzymy plik fizycznie
		file.open(file_name, std::fstream::in);
		file.close();
		std::cout << "File " << file_name << " created" << std::endl;
	}

}

void file_system::rename_file(std::string old_file_name, std::string new_file_name)
{
	int inode_index = search_inode(old_file_name);
	if (inode_index != -1)
	{
		inode_table[inode_index].file_name = new_file_name;
		std::cout << "File " << old_file_name << " renamed to " << new_file_name << std::endl;
	}
	else
	{
		std::cout << "File " << old_file_name << " doesn't exists" << std::endl;
	}

}
void file_system::rename_file_v2(std::string old_file_name, std::string new_file_name)
{
	int inode_index = search_inode(old_file_name);
	if (inode_index != -1)
	{
		inode_table[inode_index].file_name = new_file_name;
		rename(old_file_name.c_str(), new_file_name.c_str()); //zmieniamy nazwe fizycznego pliku
		std::cout << "File " << old_file_name << " renamed to " << new_file_name << std::endl;

	}
	else
	{
		std::cout << "File " << old_file_name << " doesn't exists" << std::endl;
	}

}
void file_system::add_in_data_to_file(std::string file_name, std::string data)
{
	int inode_index = search_inode(file_name);
	if (inode_index != -1)
	{
		if (is_enough_size(data))
		{
			if (data.length() <= SIZE_OF_BLOCK) //dane zajmuj?ce mniej ni? jeden blok
			{
				int free_block = search_free_block();
				if (inode_table[inode_index].first_data_block == nullptr) //je?eli w pliku nie ma nic zapisanego
				{
					for (int i = 0; i < data.length(); i++)
					{
						disk_space[free_block][i] = data.at(i);
					}
					number_of_free_blocks--;
					inode_table[inode_index].occupied_blocks_size++;
					inode_table[inode_index].first_data_block_index = free_block;
					inode_table[inode_index].first_data_block = &disk_space[free_block];
					inode_table[inode_index].file_size += data.length();
					free_drive_blocks[free_block] = 0; //ustawiamy odpowiednia pozycje we free_drive_blocks na 0- blok jest zajety
				}
				else if (inode_table[inode_index].second_data_block == nullptr) //je?eli pierwszy blok jest zapisany zapisujemy do drugiego
				{
					for (int i = 0; i < data.length(); i++)
					{
						disk_space[free_block][i] = data.at(i);
					}
					number_of_free_blocks--;
					inode_table[inode_index].occupied_blocks_size++;
					inode_table[inode_index].second_data_block_index = free_block;
					inode_table[inode_index].second_data_block = &disk_space[free_block];
					inode_table[inode_index].file_size += data.length();
					free_drive_blocks[free_block] = 0; //ustawiamy odpowiednia pozycje we free_drive_blocks na 0- blok jest zajety
				}
				else //je?eli bloki szybkiego dostepu s? ju? zaj?te
				{
					for (int i = 0; i < data.length(); i++)
					{
						disk_space[free_block][i] = data.at(i);
					}
					number_of_free_blocks--;
					inode_table[inode_index].occupied_blocks_size++;
					inode_table[inode_index].index_block.push_back(free_block);
					inode_table[inode_index].file_size += data.length();
					free_drive_blocks[free_block] = 0;
				}
			}
			else if (data.length() <= 2 * SIZE_OF_BLOCK) //kiedy dane zajmuj? 1 blok <dane<=2bloki
			{
				std::vector<int> free_blocks = search_free_blocks(2);
				if ((inode_table[inode_index].first_data_block == nullptr) and (inode_table[inode_index].second_data_block == nullptr)) //je?eli w pliku nie ma nic zapisanego
				{
					int bytes_counter = 0; //zmienna pomocnicza do obslugi data
					for (int i = 0; i < free_blocks.size(); i++)
					{
						for (int j = 0; j < SIZE_OF_BLOCK and bytes_counter < data.size(); j++, bytes_counter++) //gdy skonczy sie nam miejsce w bloku lub skoncza sie dane do zapisania
						{
							disk_space[free_blocks[i]][j] = data.at(bytes_counter);
						}
						number_of_free_blocks--;
						inode_table[inode_index].occupied_blocks_size++;
						free_drive_blocks[free_blocks[i]] = 0;
					}
					inode_table[inode_index].first_data_block_index = free_blocks[0];
					inode_table[inode_index].first_data_block = &disk_space[free_blocks[0]];
					inode_table[inode_index].second_data_block = &disk_space[free_blocks[1]];
					inode_table[inode_index].second_data_block_index = free_blocks[1];
					inode_table[inode_index].file_size += data.length();
				}
				else if (inode_table[inode_index].second_data_block == nullptr)
				{
					int bytes_counter = 0;
					for (int i = 0; i < free_blocks.size(); i++)
					{
						for (int j = 0; j < SIZE_OF_BLOCK and bytes_counter < data.size(); j++, bytes_counter++)
						{
							disk_space[free_blocks[i]][j] = data.at(bytes_counter);
						}
						number_of_free_blocks--;
						inode_table[inode_index].occupied_blocks_size++;
						free_drive_blocks[free_blocks[i]] = 0;
					}
					inode_table[inode_index].second_data_block_index = free_blocks[0];
					inode_table[inode_index].second_data_block = &disk_space[free_blocks[0]];
					inode_table[inode_index].index_block.push_back(free_blocks[1]);
					inode_table[inode_index].file_size += data.length();
				}
				else //je?eli bloki szybkiego dostepu s? ju? zaj?te
				{
					int bytes_counter = 0;
					for (int i = 0; i < free_blocks.size(); i++)
					{
						for (int j = 0; j < SIZE_OF_BLOCK and bytes_counter < data.size(); j++, bytes_counter++)
						{
							disk_space[free_blocks[i]][j] = data.at(bytes_counter);
						}
						number_of_free_blocks--;
						inode_table[inode_index].occupied_blocks_size++;
						free_drive_blocks[free_blocks[i]] = 0;
						inode_table[inode_index].index_block.push_back(free_blocks[i]);
					}
					inode_table[inode_index].file_size += data.length();
				}
			}
			else if (data.length() > 2 * SIZE_OF_BLOCK)
			{
				std::vector<int> free_blocks = search_free_blocks(ceil((float)(data.length() / (float)SIZE_OF_BLOCK))); //wyszukujemy ile blok?w potrzebujemy
				if ((inode_table[inode_index].first_data_block == nullptr) and (inode_table[inode_index].second_data_block == nullptr)) //je?eli w pliku nie ma nic zapisanego
				{
					int bytes_counter = 0;
					for (int i = 0; i < free_blocks.size(); i++)
					{
						for (int j = 0; j < SIZE_OF_BLOCK and bytes_counter < data.size(); j++, bytes_counter++)
						{
							disk_space[free_blocks[i]][j] = data.at(bytes_counter);
						}
						number_of_free_blocks--;
						inode_table[inode_index].occupied_blocks_size++;
						free_drive_blocks[free_blocks[i]] = 0;
						if (i == 0) //dla pierwszego indeksowego
						{
							inode_table[inode_index].first_data_block_index = free_blocks[i];
							inode_table[inode_index].first_data_block = &disk_space[free_blocks[i]];
						}
						else if (i == 1) //dla drugiego indeksowego
						{
							inode_table[inode_index].second_data_block_index = free_blocks[i];
							inode_table[inode_index].second_data_block = &disk_space[free_blocks[i]];
						}
						else
						{
							inode_table[inode_index].index_block.push_back(free_blocks[i]);
						}
					}
					inode_table[inode_index].file_size += data.length();
				}
				else if (inode_table[inode_index].second_data_block == nullptr)
				{
					int bytes_counter = 0;
					for (int i = 0; i < free_blocks.size(); i++)
					{
						for (int j = 0; j < SIZE_OF_BLOCK and bytes_counter < data.size(); j++, bytes_counter++)
						{
							disk_space[free_blocks[i]][j] = data.at(bytes_counter);
						}
						number_of_free_blocks--;
						inode_table[inode_index].occupied_blocks_size++;
						free_drive_blocks[free_blocks[i]] = 0;
					}
					inode_table[inode_index].second_data_block = &disk_space[free_blocks[0]];
					inode_table[inode_index].second_data_block_index = free_blocks[0];
					for (int i = 1; i < free_blocks.size(); i++) //poniewaz pierwszy wolny blok ju? przypisali?my
					{
						inode_table[inode_index].index_block.push_back(free_blocks[i]);
					}
					inode_table[inode_index].file_size += data.length();
				}
				else //je?eli bloki szybkiego dostepu s? ju? zaj?te
				{
					int bytes_counter = 0;
					for (int i = 0; i < free_blocks.size(); i++)
					{
						for (int j = 0; j < SIZE_OF_BLOCK and bytes_counter < data.size(); j++, bytes_counter++)
						{
							disk_space[free_blocks[i]][j] = data.at(bytes_counter);
						}
						number_of_free_blocks--;
						inode_table[inode_index].occupied_blocks_size++;
						free_drive_blocks[free_blocks[i]] = 0;
					}
					for (int i = 0; i < free_blocks.size(); i++)
					{
						inode_table[inode_index].index_block.push_back(free_blocks[i]);
						inode_table[inode_index].file_size += data.length();
					}
				}
			}
		}
		else
		{
			std::cout << "Not enough free space on disk. Delete other files" << std::endl;
		}
	}
	else
	{
		std::cout << "File " << file_name << " doesn't exists " << std::endl;
	}
}
void file_system::add_in_data_to_file_overwrite(std::string file_name, std::string data)
{
	std::fstream file;
	file.open(file_name.c_str(), std::ios::out | std::ios::trunc);
	int inode_index = search_inode(file_name);
	if (inode_index != -1)
	{
		if (file.is_open())
		{

			if (is_enough_size(data))
			{
				if (data.length() <= SIZE_OF_BLOCK) //dane zajmuj?ce mniej ni? jeden blok
				{
					int free_block = search_free_block();
					if (inode_table[inode_index].first_data_block == nullptr) //je?eli w pliku nie ma nic zapisanego
					{
						for (int i = 0; i < data.length(); i++)
						{
							disk_space[free_block][i] = data.at(i);
							file << data.at(i); //zapisujemy fizycznie do pliku
						}
						number_of_free_blocks--;
						inode_table[inode_index].occupied_blocks_size++;
						inode_table[inode_index].first_data_block_index = free_block;
						inode_table[inode_index].first_data_block = &disk_space[free_block];
						inode_table[inode_index].file_size += data.length();
						free_drive_blocks[free_block] = 0; //ustawiamy odpowiednia pozycje we free_drive_blocks na 0- blok jest zajety
					}
					else if (inode_table[inode_index].second_data_block == nullptr) //je?eli pierwszy blok jest zapisany zapisujemy do drugiego
					{
						for (int i = 0; i < data.length(); i++)
						{
							disk_space[free_block][i] = data.at(i);
							file << data.at(i);
						}
						number_of_free_blocks--;
						inode_table[inode_index].occupied_blocks_size++;
						inode_table[inode_index].second_data_block_index = free_block;
						inode_table[inode_index].second_data_block = &disk_space[free_block];
						inode_table[inode_index].file_size += data.length();
						free_drive_blocks[free_block] = 0;
					}
					else
					{
						for (int i = 0; i < data.length(); i++)
						{
							disk_space[free_block][i] = data.at(i);
							file << data.at(i);
						}
						number_of_free_blocks--;
						inode_table[inode_index].occupied_blocks_size++;
						inode_table[inode_index].index_block.push_back(free_block);
						inode_table[inode_index].file_size += data.length();
						free_drive_blocks[free_block] = 0;
					}
				}
				else if (data.length() <= 2 * SIZE_OF_BLOCK) //kiedy dane zajmuj? 1 blok <dane<=2bloki
				{
					std::vector<int> free_blocks = search_free_blocks(2);
					if ((inode_table[inode_index].first_data_block == nullptr) and (inode_table[inode_index].second_data_block == nullptr)) //je?eli w pliku nie ma nic zapisanego
					{
						int bytes_counter = 0; //zmienna pomocnicza do obslugi data
						for (int i = 0; i < free_blocks.size(); i++)
						{
							for (int j = 0; j < SIZE_OF_BLOCK and bytes_counter < data.size(); j++, bytes_counter++) //gdy skonczy sie nam miejsce w bloku lub skoncza sie dane do zapisania
							{
								disk_space[free_blocks[i]][j] = data.at(bytes_counter);
								file << data.at(bytes_counter);
							}
							number_of_free_blocks--;
							inode_table[inode_index].occupied_blocks_size++;
							free_drive_blocks[free_blocks[i]] = 0;
						}
						inode_table[inode_index].first_data_block_index = free_blocks[0];
						inode_table[inode_index].first_data_block = &disk_space[free_blocks[0]];
						inode_table[inode_index].second_data_block = &disk_space[free_blocks[1]];
						inode_table[inode_index].second_data_block_index = free_blocks[1];
						inode_table[inode_index].file_size += data.length();
					}
					else if (inode_table[inode_index].second_data_block == nullptr)
					{
						int bytes_counter = 0;
						for (int i = 0; i < free_blocks.size(); i++)
						{
							for (int j = 0; j < SIZE_OF_BLOCK and bytes_counter < data.size(); j++, bytes_counter++)
							{
								disk_space[free_blocks[i]][j] = data.at(bytes_counter);
								file << data.at(bytes_counter);
							}
							number_of_free_blocks--;
							inode_table[inode_index].occupied_blocks_size++;
							free_drive_blocks[free_blocks[i]] = 0;
						}
						inode_table[inode_index].second_data_block_index = free_blocks[0];
						inode_table[inode_index].second_data_block = &disk_space[free_blocks[0]];
						inode_table[inode_index].index_block.push_back(free_blocks[1]);
						inode_table[inode_index].file_size += data.length();
					}
					else
					{
						int bytes_counter = 0;
						for (int i = 0; i < free_blocks.size(); i++)
						{
							for (int j = 0; j < SIZE_OF_BLOCK and bytes_counter < data.size(); j++, bytes_counter++)
							{
								disk_space[free_blocks[i]][j] = data.at(bytes_counter);
								file << data.at(bytes_counter);
							}
							number_of_free_blocks--;
							inode_table[inode_index].occupied_blocks_size++;
							free_drive_blocks[free_blocks[i]] = 0;
							inode_table[inode_index].index_block.push_back(free_blocks[i]);
						}
						inode_table[inode_index].file_size += data.length();
					}
				}
				else if (data.length() > 2 * SIZE_OF_BLOCK)
				{
					std::vector<int> free_blocks = search_free_blocks(ceil((float)(data.length() / (float)SIZE_OF_BLOCK))); //wyszukujemy ile blok?w potrzebujemy
					if ((inode_table[inode_index].first_data_block == nullptr) and (inode_table[inode_index].second_data_block == nullptr)) //je?eli w pliku nie ma nic zapisanego
					{
						int bytes_counter = 0;
						for (int i = 0; i < free_blocks.size(); i++)
						{
							for (int j = 0; j < SIZE_OF_BLOCK and bytes_counter < data.size(); j++, bytes_counter++)
							{
								disk_space[free_blocks[i]][j] = data.at(bytes_counter);
								file << data.at(bytes_counter);
							}
							number_of_free_blocks--;
							inode_table[inode_index].occupied_blocks_size++;
							free_drive_blocks[free_blocks[i]] = 0;
							if (i == 0) //dla pierwszego indeksowego
							{
								inode_table[inode_index].first_data_block_index = free_blocks[i];
								inode_table[inode_index].first_data_block = &disk_space[free_blocks[i]];
							}
							else if (i == 1) //dla drugiego indeksowego
							{
								inode_table[inode_index].second_data_block_index = free_blocks[i];
								inode_table[inode_index].second_data_block = &disk_space[free_blocks[i]];
							}
							else
							{
								inode_table[inode_index].index_block.push_back(free_blocks[i]);
							}
						}
						inode_table[inode_index].file_size += data.length();
					}
					else if (inode_table[inode_index].second_data_block == nullptr) //je?eli pierwszy blok jest zapisany zapisujemy do drugiego i indeksowego
					{
						int bytes_counter = 0;
						for (int i = 0; i < free_blocks.size(); i++)
						{
							for (int j = 0; j < SIZE_OF_BLOCK and bytes_counter < data.size(); j++, bytes_counter++)
							{
								disk_space[free_blocks[i]][j] = data.at(bytes_counter);
								file << data.at(bytes_counter);
							}
							number_of_free_blocks--;
							inode_table[inode_index].occupied_blocks_size++;
							free_drive_blocks[free_blocks[i]] = 0;
						}
						inode_table[inode_index].second_data_block = &disk_space[free_blocks[0]];
						inode_table[inode_index].second_data_block_index = free_blocks[0];
						for (int i = 1; i < free_blocks.size(); i++) //poniewaz pierwszy wolny blok ju? przypisali?my
						{
							inode_table[inode_index].index_block.push_back(free_blocks[i]);
						}
						inode_table[inode_index].file_size += data.length();
					}
					else //je?eli bloki szybkiego dostepu s? ju? zaj?te
					{
						int bytes_counter = 0;
						for (int i = 0; i < free_blocks.size(); i++)
						{
							for (int j = 0; j < SIZE_OF_BLOCK and bytes_counter < data.size(); j++, bytes_counter++)
							{
								disk_space[free_blocks[i]][j] = data.at(bytes_counter);
								file << data.at(bytes_counter);
							}
							number_of_free_blocks--;
							inode_table[inode_index].occupied_blocks_size++;
							free_drive_blocks[free_blocks[i]] = 0;
						}
						for (int i = 0; i < free_blocks.size(); i++)
						{
							inode_table[inode_index].index_block.push_back(free_blocks[i]);
							inode_table[inode_index].file_size += data.length();
						}
					}
				}
			}
			else
			{
				std::cout << "Not enough free space on disk. Delete other files" << std::endl;
			}
		}
		else
		{
			std::cout << "Error while opening file" << std::endl;
		}
	}
	else
	{
		std::cout << "File " << file_name << " doesn't exists " << std::endl;
	}
	std::cout << "Data added to file: " << file_name << std::endl;
	file.close(); //zamykamy plik
}
void file_system::add_in_data_to_file_v2(std::string file_name, std::string data)
{
	std::fstream file;
	file.open(file_name.c_str(), std::ios::out | std::ios::app);
	int inode_index = search_inode(file_name);
	if (inode_index != -1)
	{
		if (file.is_open())
		{

			if (is_enough_size(data))
			{
				if (data.length() <= SIZE_OF_BLOCK) //dane zajmuj?ce mniej ni? jeden blok
				{
					int free_block = search_free_block();
					if (inode_table[inode_index].first_data_block == nullptr) //je?eli w pliku nie ma nic zapisanego
					{
						for (int i = 0; i < data.length(); i++)
						{
							disk_space[free_block][i] = data.at(i);
							file << data.at(i); //zapisujemy fizycznie do pliku
						}
						number_of_free_blocks--;
						inode_table[inode_index].occupied_blocks_size++;
						inode_table[inode_index].first_data_block_index = free_block;
						inode_table[inode_index].first_data_block = &disk_space[free_block];
						inode_table[inode_index].file_size += data.length();
						free_drive_blocks[free_block] = 0; //ustawiamy odpowiednia pozycje we free_drive_blocks na 0- blok jest zajety
					}
					else if (inode_table[inode_index].second_data_block == nullptr) //je?eli pierwszy blok jest zapisany zapisujemy do drugiego
					{
						for (int i = 0; i < data.length(); i++)
						{
							disk_space[free_block][i] = data.at(i);
							file << data.at(i);
						}
						number_of_free_blocks--;
						inode_table[inode_index].occupied_blocks_size++;
						inode_table[inode_index].second_data_block_index = free_block;
						inode_table[inode_index].second_data_block = &disk_space[free_block];
						inode_table[inode_index].file_size += data.length();
						free_drive_blocks[free_block] = 0;
					}
					else
					{
						for (int i = 0; i < data.length(); i++)
						{
							disk_space[free_block][i] = data.at(i);
							file << data.at(i);
						}
						number_of_free_blocks--;
						inode_table[inode_index].occupied_blocks_size++;
						inode_table[inode_index].index_block.push_back(free_block);
						inode_table[inode_index].file_size += data.length();
						free_drive_blocks[free_block] = 0;
					}
				}
				else if (data.length() <= 2 * SIZE_OF_BLOCK) //kiedy dane zajmuj? 1 blok <dane<=2bloki
				{
					std::vector<int> free_blocks = search_free_blocks(2);
					if ((inode_table[inode_index].first_data_block == nullptr) and (inode_table[inode_index].second_data_block == nullptr)) //je?eli w pliku nie ma nic zapisanego
					{
						int bytes_counter = 0; //zmienna pomocnicza do obslugi data
						for (int i = 0; i < free_blocks.size(); i++)
						{
							for (int j = 0; j < SIZE_OF_BLOCK and bytes_counter < data.size(); j++, bytes_counter++) //gdy skonczy sie nam miejsce w bloku lub skoncza sie dane do zapisania
							{
								disk_space[free_blocks[i]][j] = data.at(bytes_counter);
								file << data.at(bytes_counter);
							}
							number_of_free_blocks--;
							inode_table[inode_index].occupied_blocks_size++;
							free_drive_blocks[free_blocks[i]] = 0;
						}
						inode_table[inode_index].first_data_block_index = free_blocks[0];
						inode_table[inode_index].first_data_block = &disk_space[free_blocks[0]];
						inode_table[inode_index].second_data_block = &disk_space[free_blocks[1]];
						inode_table[inode_index].second_data_block_index = free_blocks[1];
						inode_table[inode_index].file_size += data.length();
					}
					else if (inode_table[inode_index].second_data_block == nullptr)
					{
						int bytes_counter = 0;
						for (int i = 0; i < free_blocks.size(); i++)
						{
							for (int j = 0; j < SIZE_OF_BLOCK and bytes_counter < data.size(); j++, bytes_counter++)
							{
								disk_space[free_blocks[i]][j] = data.at(bytes_counter);
								file << data.at(bytes_counter);
							}
							number_of_free_blocks--;
							inode_table[inode_index].occupied_blocks_size++;
							free_drive_blocks[free_blocks[i]] = 0;
						}
						inode_table[inode_index].second_data_block_index = free_blocks[0];
						inode_table[inode_index].second_data_block = &disk_space[free_blocks[0]];
						inode_table[inode_index].index_block.push_back(free_blocks[1]);
						inode_table[inode_index].file_size += data.length();
					}
					else
					{
						int bytes_counter = 0;
						for (int i = 0; i < free_blocks.size(); i++)
						{
							for (int j = 0; j < SIZE_OF_BLOCK and bytes_counter < data.size(); j++, bytes_counter++)
							{
								disk_space[free_blocks[i]][j] = data.at(bytes_counter);
								file << data.at(bytes_counter);
							}
							number_of_free_blocks--;
							inode_table[inode_index].occupied_blocks_size++;
							free_drive_blocks[free_blocks[i]] = 0;
							inode_table[inode_index].index_block.push_back(free_blocks[i]);
						}
						inode_table[inode_index].file_size += data.length();
					}
				}
				else if (data.length() > 2 * SIZE_OF_BLOCK)
				{
					std::vector<int> free_blocks = search_free_blocks(ceil((float)(data.length() / (float)SIZE_OF_BLOCK))); //wyszukujemy ile blok?w potrzebujemy
					if ((inode_table[inode_index].first_data_block == nullptr) and (inode_table[inode_index].second_data_block == nullptr)) //je?eli w pliku nie ma nic zapisanego
					{
						int bytes_counter = 0;
						for (int i = 0; i < free_blocks.size(); i++)
						{
							for (int j = 0; j < SIZE_OF_BLOCK and bytes_counter < data.size(); j++, bytes_counter++)
							{
								disk_space[free_blocks[i]][j] = data.at(bytes_counter);
								file << data.at(bytes_counter);
							}
							number_of_free_blocks--;
							inode_table[inode_index].occupied_blocks_size++;
							free_drive_blocks[free_blocks[i]] = 0;
							if (i == 0) //dla pierwszego indeksowego
							{
								inode_table[inode_index].first_data_block_index = free_blocks[i];
								inode_table[inode_index].first_data_block = &disk_space[free_blocks[i]];
							}
							else if (i == 1) //dla drugiego indeksowego
							{
								inode_table[inode_index].second_data_block_index = free_blocks[i];
								inode_table[inode_index].second_data_block = &disk_space[free_blocks[i]];
							}
							else
							{
								inode_table[inode_index].index_block.push_back(free_blocks[i]);
							}
						}
						inode_table[inode_index].file_size += data.length();
					}
					else if (inode_table[inode_index].second_data_block == nullptr) //je?eli pierwszy blok jest zapisany zapisujemy do drugiego i indeksowego
					{
						int bytes_counter = 0;
						for (int i = 0; i < free_blocks.size(); i++)
						{
							for (int j = 0; j < SIZE_OF_BLOCK and bytes_counter < data.size(); j++, bytes_counter++)
							{
								disk_space[free_blocks[i]][j] = data.at(bytes_counter);
								file << data.at(bytes_counter);
							}
							number_of_free_blocks--;
							inode_table[inode_index].occupied_blocks_size++;
							free_drive_blocks[free_blocks[i]] = 0;
						}
						inode_table[inode_index].second_data_block = &disk_space[free_blocks[0]];
						inode_table[inode_index].second_data_block_index = free_blocks[0];
						for (int i = 1; i < free_blocks.size(); i++) //poniewaz pierwszy wolny blok ju? przypisali?my
						{
							inode_table[inode_index].index_block.push_back(free_blocks[i]);
						}
						inode_table[inode_index].file_size += data.length();
					}
					else //je?eli bloki szybkiego dostepu s? ju? zaj?te
					{
						int bytes_counter = 0;
						for (int i = 0; i < free_blocks.size(); i++)
						{
							for (int j = 0; j < SIZE_OF_BLOCK and bytes_counter < data.size(); j++, bytes_counter++)
							{
								disk_space[free_blocks[i]][j] = data.at(bytes_counter);
								file << data.at(bytes_counter);
							}
							number_of_free_blocks--;
							inode_table[inode_index].occupied_blocks_size++;
							free_drive_blocks[free_blocks[i]] = 0;
						}
						for (int i = 0; i < free_blocks.size(); i++)
						{
							inode_table[inode_index].index_block.push_back(free_blocks[i]);
							inode_table[inode_index].file_size += data.length();
						}
					}
				}
			}
			else
			{
				std::cout << "Not enough free space on disk. Delete other files" << std::endl;
			}
		}
		else
		{
			std::cout << "Error while opening file" << std::endl;
		}
	}
	else
	{
		std::cout << "File " << file_name << " doesn't exists " << std::endl;
	}
	std::cout << "Data added to file: " << file_name << std::endl;
	file.close(); //zamykamy plik
}

void file_system::free_up_block_for_index(int block)
{
	for (int j = 0; j < SIZE_OF_BLOCK; j++)
	{
		disk_space[block][j] = -1;
	}
	free_drive_blocks[block] = 1;//zmieniamy wartosc w wektorze tablic na 1- blok gotowy do zapisu
	number_of_free_blocks++;
}
void file_system::free_up_block_for_pointer(std::vector<char>* pointer, int pointer_block_index)
{

	for (int j = 0; j < SIZE_OF_BLOCK; j++)
	{
		(*pointer)[j] = -1;
	}
	free_drive_blocks[pointer_block_index] = 1;//zmieniamy wartosc w wektorze tablic na 1- blok gotowy do zapisu
	number_of_free_blocks++;
}
void file_system::delete_file(std::string file_name)
{
	int inode_index = search_inode(file_name);
	if (inode_index != -1)
	{
		//czyscimy bloki na dysku
		if (inode_table[inode_index].first_data_block != nullptr)
		{
			free_up_block_for_pointer(inode_table[inode_index].first_data_block, inode_table[inode_index].first_data_block_index); //czyscimy 1 blok "szybkiego dostepu"
		}
		if (inode_table[inode_index].second_data_block != nullptr)
		{
			free_up_block_for_pointer(inode_table[inode_index].second_data_block, inode_table[inode_index].second_data_block_index); //czyscimy 2 blok "szybkiego dostepu"
		}
		if (!inode_table[inode_index].index_block.empty())
		{
			for (int i = 0, j = 0; i < inode_table[inode_index].index_block.size(); i++)
			{
				free_up_block_for_index(inode_table[inode_index].index_block[i]); //czyscimy bloki indeksowe
			}
		}
		inode_table.erase(inode_table.begin() + inode_index);//usuwamy iwezel
	}
	else
	{
		std::cout << "File " << file_name << " doesn't exists " << std::endl;
	}


}
void file_system::delete_file_v2(std::string file_name)
{
	int inode_index = search_inode(file_name);
	if (inode_index != -1)
	{
		//czyscimy bloki na dysku
		if (inode_table[inode_index].first_data_block != nullptr)
		{
			free_up_block_for_pointer(inode_table[inode_index].first_data_block, inode_table[inode_index].first_data_block_index); //czyscimy 1 blok "szybkiego dostepu"
		}
		if (inode_table[inode_index].second_data_block != nullptr)
		{
			free_up_block_for_pointer(inode_table[inode_index].second_data_block, inode_table[inode_index].second_data_block_index); //czyscimy 2 blok "szybkiego dostepu"
		}
		if (!inode_table[inode_index].index_block.empty())
		{
			for (int i = 0, j = 0; i < inode_table[inode_index].index_block.size(); i++)
			{
				free_up_block_for_index(inode_table[inode_index].index_block[i]); //czyscimy bloki indeksowe
			}
		}
		inode_table.erase(inode_table.begin() + inode_index);//usuwamy iwezel
		remove(file_name.c_str()); //usuwamy plik
	}
	else
	{
		std::cout << "File " << file_name << " doesn't exists " << std::endl;
	}
}

void file_system::overwrite_data_to_file(std::string file_name, std::string data)
{
	//wpierw usuwamy dane z dysku
	int inode_index = search_inode(file_name);
	if (inode_index != -1)
	{
		if (is_enough_size(data, inode_table[inode_index].file_size))
		{
			//czyscimy bloki na dysku
			if (inode_table[inode_index].first_data_block != nullptr)
			{
				free_up_block_for_pointer(inode_table[inode_index].first_data_block, inode_table[inode_index].first_data_block_index); //czyscimy 1 blok "szybkiego dostepu"
			}
			if (inode_table[inode_index].second_data_block != nullptr)
			{
				free_up_block_for_pointer(inode_table[inode_index].second_data_block, inode_table[inode_index].second_data_block_index); //czyscimy 2 blok "szybkiego dostepu"
			}
			if (!inode_table[inode_index].index_block.empty())
			{
				for (int i = 0, j = 0; i < inode_table[inode_index].index_block.size(); i++)
				{
					free_up_block_for_index(inode_table[inode_index].index_block[i]); //czyscimy bloki indeksowe
				}
			}
			inode_table[inode_index].file_size = 0;
			inode_table[inode_index].first_data_block = nullptr;
			inode_table[inode_index].second_data_block = nullptr;
			inode_table[inode_index].first_data_block_index = -1;
			inode_table[inode_index].second_data_block_index = -1;
			inode_table[inode_index].index_block.clear();
			add_in_data_to_file(file_name, data); //bo usunieciu starych danych pliku i wyzerowaniu jego meta-daty dopisujemy do pliku nowe informacje
		}
		else
		{
			std::cout << "Not enough free space on disk. Delete other files" << std::endl;
		}
	}
	else
	{
		std::cout << "File " << file_name << " doesn't exists " << std::endl;
	}

}
void file_system::overwrite_data_to_file_v2(std::string file_name, std::string data)
{
	//wpierw usuwamy dane z dysku
	int inode_index = search_inode(file_name);
	if (inode_index != -1)
	{
		if (is_enough_size(data, inode_table[inode_index].file_size))
		{
			//czyscimy bloki na dysku
			if (inode_table[inode_index].first_data_block != nullptr)
			{
				free_up_block_for_pointer(inode_table[inode_index].first_data_block, inode_table[inode_index].first_data_block_index); //czyscimy 1 blok "szybkiego dostepu"
			}
			if (inode_table[inode_index].second_data_block != nullptr)
			{
				free_up_block_for_pointer(inode_table[inode_index].second_data_block, inode_table[inode_index].second_data_block_index); //czyscimy 2 blok "szybkiego dostepu"
			}
			if (!inode_table[inode_index].index_block.empty())
			{
				for (int i = 0, j = 0; i < inode_table[inode_index].index_block.size(); i++)
				{
					free_up_block_for_index(inode_table[inode_index].index_block[i]); //czyscimy bloki indeksowe
				}
			}
			inode_table[inode_index].file_size = 0;
			inode_table[inode_index].first_data_block = nullptr;
			inode_table[inode_index].second_data_block = nullptr;
			inode_table[inode_index].first_data_block_index = -1;
			inode_table[inode_index].second_data_block_index = -1;
			inode_table[inode_index].index_block.clear();

			//delete_file_v2(file_name);
			//create_file_v2(file_name);


			add_in_data_to_file_overwrite(file_name, data);
		}
		else
		{
			std::cout << "Not enough free space on disk. Delete other files" << std::endl;
		}
	}
	else
	{
		std::cout << "File " << file_name << " doesn't exists " << std::endl;
	}
}
const void file_system::display_file(std::string file_name)
{
	int inode_index = search_inode(file_name);
	if (inode_index != -1)
	{
		if (inode_table[inode_index].first_data_block != nullptr) //jezeli pierwzszy blok wskaznikowy nie jest pusty
		{
			for (int i = 0; i < SIZE_OF_BLOCK; i++)
			{
				if ((*inode_table[inode_index].first_data_block)[i] == -1) //gdy dojdziemy do niezapisanego elementu bloku przerywamy
				{
					break;
				}
				std::cout << static_cast<char>((*inode_table[inode_index].first_data_block)[i]); //wyswietlamy zawartosc
			}
		}
		if (inode_table[inode_index].second_data_block != nullptr)
		{
			for (int i = 0; i < SIZE_OF_BLOCK; i++)
			{
				if ((*inode_table[inode_index].first_data_block)[i] == -1)
				{
					break;
				}
				std::cout << static_cast<char>((*inode_table[inode_index].second_data_block)[i]);
			}
		}
		if (!inode_table[inode_index].index_block.empty()) //je?eli vector blokow indeksowych nie jest pusty
		{
			for (int i = 0; i < inode_table[inode_index].index_block.size(); i++)
			{
				for (int j = 0; j < SIZE_OF_BLOCK; j++)
				{
					if (disk_space[inode_table[inode_index].index_block[i]][j] == -1)
					{
						break;
					}
					std::cout << static_cast<char>(disk_space[inode_table[inode_index].index_block[i]][j]);
				}
			}
		}
		std::cout << std::endl;
	}
	else
	{
		std::cout << "File " << file_name << " doesn't exists " << std::endl;
	}
}
const void file_system::display_file_v2(std::string file_name)
{
	std::fstream file;
	file.open(file_name.c_str(), std::ios::in);
	std::string help;
	if (file.is_open())
	{
		while (!file.eof())
		{
			getline(file, help);
			std::cout << help << std::endl;
		}
	}
	else
	{
		std::cout << "Error while opening file" << std::endl;
	}
	file.close();
}
std::string file_system::return_file_as_string(std::string file_name)
{
	std::fstream file;
	file.open(file_name.c_str(), std::ios::in);
	std::string help, returning;
	if (file.is_open())
	{
		while (!file.eof())
		{
			getline(file, help);
			returning += help;
		}
	}
	else
	{
		std::cout << "Error while opening file" << std::endl;
	}
	file.close();
	return returning;
}
const void file_system::display_part_of_file(std::string file_name, int start_pos, int nr_of_characters)
{
	std::fstream file;
	file.open(file_name.c_str(), std::ios::in);
	file.seekg(start_pos, std::ios::beg);
	char* buffer = new char[nr_of_characters];
	file.read(buffer, nr_of_characters);
	for (int i = 0; i < nr_of_characters; i++)
	{
		std::cout << buffer[i];
	}
	file.close();
	delete[] buffer;

}
std::string file_system::read_part_of_file(std::string file_name, int start_pos, int nr_of_characters)
{
	std::fstream file;
	std::string returning;
	file.open(file_name.c_str(), std::ios::in);
	file.seekg(start_pos, std::ios::beg);
	char* buffer = new char[nr_of_characters];
	file.read(buffer, nr_of_characters);
	for (int i = 0; i < nr_of_characters; i++)
	{
		returning += buffer[i];
	}
	file.close();
	delete[] buffer;
	return returning;
}
char file_system::return_single_char(std::string file_name, int start_pos)
{
	std::fstream file;
	char returning;
	file.open(file_name.c_str(), std::ios::in);
	file.seekg(start_pos);
	file.read(&returning, 1);
	return returning;
	file.close();

}/*

//Synchronizacja, Jan Witczak
void file_system::open_file(std::string file_name_)
{
	int inode_index = search_inode(file_name_);
	if (inode_index != -1)
	{
		inode_table[inode_index].File_Mutex.wait(true);
		inode_table[inode_index].writing = true;
	}
}

void file_system::close_file(std::string file_name_)
{
	int inode_index = search_inode(file_name_);
	if (inode_index != -1)
	{
		inode_table[inode_index].File_Mutex.signal();
		inode_table[inode_index].writing = false;
	}
}

void file_system::open_file_reading(std::string file_name_)
{
	int inode_index = search_inode(file_name_);
	if (inode_index != -1)
	{
		inode_table[inode_index].Read_Count++;
		if (inode_table[inode_index].Read_Count == 1 || inode_table[inode_index].writing) inode_table[inode_index].File_Mutex.wait(false);
	}
}

void file_system::close_file_reading(std::string file_name_)
{
	int inode_index = search_inode(file_name_);
	if (inode_index != -1)
	{
		inode_table[inode_index].Read_Count--;
		if (inode_table[inode_index].Read_Count == 0) inode_table[inode_index].File_Mutex.signal();
	}
}
//Synchronizacja. */
int main()
{
	file_system a;
	bool running = true;
	int number;
	std::string file_name;
	std::string data;
	std::string new_file_name;
	while (running)
	{
		std::cout << "Choose operation to perform:" << std::endl;
		std::cout << "1: Create file" << std::endl << "2: Add in data to file" << std::endl << "3: Delete file" << std::endl << "4: Rename file" << std::endl << "5: Overwrite file" << std::endl << "6: Display file" << std::endl << "7: Print drive" << std::endl << "8: Display all informations about all files" << std::endl << "9: Display files in catalogue" << std::endl << "0: END" << std::endl;
		std::cin >> number;
		if (number > 9 or number < 0)
		{
			std::cout << "Wrong number entered, enter number once again" << std::endl;
			std::cin >> number;
		}
		switch (number)
		{
		case 0:
			running = false;
			break;

		case 1:
			std::cout << "Enter creating file filename" << std::endl;
			file_name.clear();
			std::cin >> file_name;
			a.create_file_v2(file_name);
			break;
		case 2:
			std::cout << "Enter filename" << std::endl;
			file_name.clear();
			std::cin >> file_name;
			std::cout << "Enter data";
			std::getline(std::cin, data);
			data.clear();
			//std::cin >> data;
			
			a.add_in_data_to_file_v2(file_name, data);
			break;
		case 3:
			std::cout << "Enter filename to delete" << std::endl;
			file_name.clear();
			std::cin >> file_name;
			a.delete_file_v2(file_name);
			break;

		case 4:
			std::cout << "Enter old filename" << std::endl;
			file_name.clear();
			std::cin >> file_name;
		
			std::cout << "Enter new filename" << std::endl;
			new_file_name.clear();
			std::cin >> new_file_name;
			a.rename_file_v2(file_name, new_file_name);
			break;
		case 5:
			std::cout << "Enter filename";
			file_name.clear();
			std::cin >> file_name;
			std::cout << "Enter data to overwrite";
			data.clear();
			std::getline(std::cin, data);
			a.overwrite_data_to_file_v2(file_name, data);
			break;
		case 6:
			std::cout << "Enter filename" << std::endl;
			file_name.clear();
			std::cin >> file_name;
			a.display_file_v2(file_name);
			break;

		case 7:
			a.print_drive();
			break;
		case 8:
			a.display_all_information_about_all_files();
			break;
		case 9:
			a.print_inode_catalogue();
			break;

		default:
			break;
		}
	}
}