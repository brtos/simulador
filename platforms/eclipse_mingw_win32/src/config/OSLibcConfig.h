/*
 * OSLibcConfig.h
 *
 */

#ifndef CONFIG_OSLIBCCONFIG_H_
#define CONFIG_OSLIBCCONFIG_H_


#define INC_STDIO_H			0
#define INC_FATFS_H			0

/* stdio config */
#if INC_STDIO_H
#include <dirent.h>
#define OS_FILETYPE                  FILE*
#define OS_FILEPOS                   fpos_t
#define OS_DIRTYPE                   DIR*
#define OS_DIRINFO 				  	 struct dirent *
#define OS_FILEINFO 				 struct stat

#define OSfopenread(filename,file)   ((*(file) = fopen((filename),"rb+")) != NULL)
#define OSfopenwrite(filename,file)  ((*(file) = fopen((filename),"wb")) != NULL)
#define OSfopenappend(filename,file) ((*(file) = fopen((filename),"ab+")) != NULL)
#define OSfclose(file)               (fclose(*(file)) == 0)
#define OSfread(buffer,size,file)    (fgets((char*)(buffer),(size),*(file)) != NULL)
#define OSfwrite(buffer,file)        (fputs((char*)(buffer),*(file)) >= 0)
#define OSrename(source,dest)       (rename((source), (dest)) == 0)
#define OSremove(filename)          (remove(filename) == 0)
#define OSftell(file,pos)            ((*(pos) = ftell(*(file))) != (-1L))
#define OSfseek(file,pos)            (fseek(*(file), *(pos), SEEK_SET) == 0)
#define OSfseek_end(file)            (fseek(*(file), 0, SEEK_END) == 0)
#define OSfseek_begin(file)          (fseek(*(file), 0, SEEK_SET) == 0)
#define OSfstat(filename, fileinfo)  (stat((filename), (fileinfo)) == 0)
#define OSopendir(dirname,dir)	     (((dir) = opendir(dirname)) != NULL)
#define OSclosedir(dir)			 closedir(dir)
#define OSreaddir(dirinfo,dir)  	 (((dirinfo) = readdir(dir)) != NULL)
#define OSchdir(dirname)			 chdir(dirname)
#define OSmkdir(dirname)			 (_mkdir(dirname) == 0)
#endif

#elif INC_FATFS_H

/* You must set _USE_STRFUNC to 1 or 2 in the include file ff.h
 * to enable the "string functions" fgets() and fputs().
 */
#include "ff.h"                       /* include ff.h for FatFs */
#define OS_BUFFERSIZE  256            /* maximum line length, maximum path length */
#define OS_FILETYPE    			  	  FIL
#define OS_FILEPOS                    DWORD
#define OS_DIRTYPE					  DIR
#define OS_DIRINFO 					  FILINFO
#define OS_FILEINFO 			      FILINFO

#define OSfopenread(filename,file)   (f_open((file), (filename), FA_READ+FA_OPEN_EXISTING) == FR_OK)
#define OSfopenwrite(filename,file)  (f_open((file), (filename), FA_WRITE+FA_CREATE_ALWAYS) == FR_OK)
#define OSfopenappend(filename,file) (f_open((file), (filename), FA_WRITE) == FR_OK)
#define OSfclose(file)               (f_close(file) == FR_OK)
#define OSfread(buffer,size,file)     f_gets((buffer), (size),(file))
#define OSfwrite(buffer,file)        (f_puts((buffer), (file)) != EOF)
#define OSrename(source,dest)
#define OSremove(filename)           (f_unlink(filename) == FR_OK)
#define OSftell(file,pos)            (*(pos) = f_tell((file)))
#define OSfseek(file,pos)            (f_lseek((file), *(pos)) == FR_OK)
#define OSfseek_end(file)            (f_lseek((file), f_size((file))) == FR_OK)
#define OSfseek_begin(file)
#define OSfstat(filename, fileinfo)  (f_stat((filename), (fileinfo)) == FR_OK)
#define OSopendir(dirname,dir)	     (f_opendir(&(dir),dirname) == FR_OK)
#define OSclosedir(dir)			     f_closedir(&(dir))
#define OSreaddir(dirinfo,dir)  	 (f_readdir(&(dir), &(dirinfo)) == FR_OK)
#define OSchdir(dirname)			  f_chdir(dirname)
#define OSmkdir(dirname)			 (f_mkdir(dirname) == FR_OK)

#else
	
#define OS_FILETYPE                  
#define OS_FILEPOS   
#define OS_DIRTYPE                     
#define OS_DIRINFO 				  	   
#define OS_FILEINFO 				   
#define OSfopenread(filename,file)   
#define OSfopenwrite(filename,file)  
#define OSfopenappend(filename,file) 
#define OSfclose(file)               
#define OSfread(buffer,size,file)    
#define OSfwrite(buffer,file)        
#define OSrename(source,dest)       
#define OSremove(filename)                           
#define OSftell(file,pos)            
#define OSfseek(file,pos)            
#define OSfseek_end(file)            
#define OSfseek_begin(file)          
#define OSfstat(filename, fileinfo)    
#define OSopendir(dirname,dir)	       
#define OSclosedir(dir)			   
#define OSreaddir(dirinfo,dir)  	   
#define OSchdir(dirname)			   
#define OSmkdir(dirname)			   

#endif


#endif /* CONFIG_OSLIBCCONFIG_H_ */
