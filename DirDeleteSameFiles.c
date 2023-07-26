#include<stdio.h>
#include<dirent.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<string.h>
#include<winbase.h>

int main(int argc, char** argv){
	
	if(argc!=3 && argc!=4)return 1;
	
	char* del_path	=argv[1]; 			   // Duplicate files which starts with this path will be deleted [required], format: "D:/SSD/Laptop/Files2/"
	char* dir_path_1=argv[2]; 			   // Folder where to get files recursively for searching duplicate files [required], format: "D:/SSD/Laptops/Files1"
	char* dir_path_2=argc==4?argv[3]:NULL; // Folder where to get files recursively for searching duplicate files [optional], format: "D:/SSD/Laptops/Files2"

	unsigned long next_file=0,max_files=0;
	char** files=malloc(max_files*sizeof(char*));

	printf("\nFiles indexing...");

	{

		unsigned long last_dir=(argc==3?0:1),max_dirs=(argc==3?1:2);
		char** dirs=malloc(max_dirs*sizeof(char*));
		strcpy((dirs[0]=malloc((strlen(dir_path_1)+1)*sizeof(char))),dir_path_1);
		if(argc==4)strcpy((dirs[1]=malloc((strlen(dir_path_2)+1)*sizeof(char))),dir_path_2);

		DIR* dir;
		char* parent_dir;
		struct dirent* dir_entry;
		unsigned long dir_dirs_count,dir_files_count;
		
		char* stat_dir;
		struct stat stat_entry;

		do

			if((dir=opendir(dirs[last_dir]))){
				
				parent_dir=dirs[last_dir];

				dir_dirs_count=dir_files_count=0;
				readdir(dir);
				readdir(dir);
				while((dir_entry=readdir(dir))!=0)
						 if(strchr(dir_entry->d_name,'?')!=0)return 2;
					else if(dir_entry->d_type==DT_DIR	 	)dir_dirs_count++;
					else if(dir_entry->d_type==DT_REG	 	)dir_files_count++;
					else if(dir_entry->d_type==DT_UNKNOWN	){
						stat_dir=strcat(strcat(strcat(calloc(strlen(parent_dir)+1+strlen(dir_entry->d_name)+1,sizeof(char)),parent_dir),"/"),dir_entry->d_name);
						if(stat(stat_dir,&stat_entry)!=0)return 3;
							 if(stat_entry.st_mode & S_IFDIR)dir_dirs_count++;
						else if(stat_entry.st_mode & S_IFREG)dir_files_count++;
						free(stat_dir);
					}
				rewinddir(dir);

				if(last_dir+dir_dirs_count>max_dirs){
					max_dirs=last_dir+dir_dirs_count;
					if((dirs=realloc(dirs,max_dirs*sizeof(char*)))==0)return 4;
				}

				if(next_file+dir_files_count>max_files){
					max_files=next_file+dir_files_count;
					if((files=realloc(files,max_files*sizeof(char*)))==0)return 5;
				}

				readdir(dir);
				readdir(dir);
				while((dir_entry=readdir(dir))!=0)
						 if(strchr(dir_entry->d_name,'?')!=0)return 6;
					else if(dir_entry->d_type==DT_DIR	 	) dirs[ last_dir++]=strcat(strcat(strcat(calloc(strlen(parent_dir)+1+strlen(dir_entry->d_name)+1,sizeof(char)),parent_dir),"/"),dir_entry->d_name);
					else if(dir_entry->d_type==DT_REG	 	)files[next_file++]=strcat(strcat(strcat(calloc(strlen(parent_dir)+1+strlen(dir_entry->d_name)+1,sizeof(char)),parent_dir),"/"),dir_entry->d_name);
					else if(dir_entry->d_type==DT_UNKNOWN	){
						stat_dir=strcat(strcat(strcat(calloc(strlen(parent_dir)+1+strlen(dir_entry->d_name)+1,sizeof(char)),parent_dir),"/"),dir_entry->d_name);
						if(stat(stat_dir,&stat_entry)!=0)return 7;
							 if(stat_entry.st_mode & S_IFDIR) dirs[ last_dir++]=strcat(strcat(strcat(calloc(strlen(parent_dir)+1+strlen(dir_entry->d_name)+1,sizeof(char)),parent_dir),"/"),dir_entry->d_name);
						else if(stat_entry.st_mode & S_IFREG)files[next_file++]=strcat(strcat(strcat(calloc(strlen(parent_dir)+1+strlen(dir_entry->d_name)+1,sizeof(char)),parent_dir),"/"),dir_entry->d_name);
						free(stat_dir);
					}

				closedir(dir);
				free(parent_dir);

			}

		while(last_dir--);

		free(dirs);

	}

	printf(" finished with %lu files:\n\n",next_file);

	{

		FILE *file1_stream,
			 *file2_stream;

		long long* files_size=malloc(next_file*sizeof(unsigned long long int));

		char file1_equals_file2;

		char *file1_buf=malloc(4096),
			 *file2_buf=malloc(4096);
		short file1_buf_read=0,
			  file2_buf_read=0;

		for(unsigned long i=0;i<next_file;i++){

			if((file1_stream=fopen(files[i],"rb"))==0)return 8;

			fseek(file1_stream,0L,SEEK_END);
			files_size[i]=ftell(file1_stream);

			for(unsigned long j=0;j<i;j++)
				if(files_size[j]>=0 && files_size[i]==files_size[j]){

					file1_equals_file2=1;

					rewind(file1_stream);
					if((file2_stream=fopen(files[j],"rb"))==0)return 9;
					while(file1_equals_file2 &&
						  (file1_buf_read=fread(file1_buf,sizeof(char),4096,file1_stream))>0 &&
						  (file2_buf_read=fread(file2_buf,sizeof(char),4096,file2_stream))>0)
						if(file1_buf_read!=file2_buf_read)return 10;
						else for(unsigned long k=0;k<file1_buf_read;k++)
							     if(file1_buf[k]!=file2_buf[k]){
									 file1_equals_file2=0;
									 break;
								 }
								
					if(fclose(file2_stream))return 11;

					if(file1_equals_file2){

						if(files_size[i]==0 || files_size[j]==0){
							
							//printf(" 0 %s\n   %s\n\n",files[j],files[i]);
							
						}
						else if(strncmp(files[i],del_path,strlen(del_path))==0){
							
							if(fclose(file1_stream))return 12;
							file1_stream=0;

							if(remove(files[i])!=0)return 13;
							files_size[i]=-1;
							printf(" ! %s\n X %s\n\n",files[j],files[i]);
							
							break;

						}
						else if(strncmp(files[j],del_path,strlen(del_path))==0){

							if(remove(files[j])!=0)return 14;
							files_size[j]=-1;
							printf(" ! %s\n X %s\n\n",files[i],files[j]);

						}
						else{
							
							//printf(" ? %s\n   %s\n\n",files[j],files[i]);
							
						}

					}

				}

			if(file1_stream!=0 && fclose(file1_stream))return 15;

		}

		free(file1_buf);
		free(file2_buf);
		free(files_size);

	}
	
	printf("operation on files is done!\n");

	return 0;

}