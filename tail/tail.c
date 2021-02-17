//------------------------------------------------------------------------
// NAME: Martin Hristev
// CLASS: XIv
// NUMBER: 13
// PROBLEM: #1
// FILE NAME: tail.c 
// FILE PURPOSE:
// да се разработи unix tail
// 
//------------------------------------------------------------------------


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

//function for standart input
int func(){
  size_t buf_size = 1024;
  char *buf = malloc(buf_size*sizeof(char));
  int read_res;
  int write_res;
  int i = 0;
  int lines = 0;
  int how_much_to_read = 0;
  int full_read = 0;
  char *p_buf;

//reads and writes to the buffer, each time it writes as much characters from the start as it has read the previous times
  while((read_res = read(STDIN_FILENO, buf+full_read, buf_size)) > 0){ 
      if (read_res == -1) {
        char fail_read[100] = "tail: error reading";
        perror(fail_read);
        return 1;
      }
      //how much characters we have read for now 
      full_read += read_res; 
  
    }

    for(i = (full_read-1); i != -1; i--){ 
                if(buf[i] == '\n' && i != (full_read-1)){
                    lines++;
                    if(lines == 10){
                      //point to the 10th line and +1 to skip the new line
                      p_buf = &buf[i+1];
                      // total bites read - those we dont need to read
                      how_much_to_read = full_read - i;
                      break; 
                    }     
                }
    }
    
    if(lines < 10){
        // point to the full buffer
        p_buf = buf; 
        // all digits we need to read 
        how_much_to_read = full_read + 1;
    }

    //write buf to STDOUT_FILENO 
    while ((write_res = write(STDOUT_FILENO, p_buf, how_much_to_read-1)) != 0) {
              if (write_res == -1)
                {
                  perror ("tail: error writing 'standard output'");
                  return 1;
                }
              p_buf += write_res;
              how_much_to_read -= write_res;
    }
    return 0;
}

int main (int argc, char *argv[])
{
  size_t buf_size = 1024;
  char buf[buf_size];
  int read_res;
  int write_res;
  int i = 0;
  int lines = 0;
  int res_close = 0;
  int how_much_to_read = 0;
  char *p_buf;
  int filename_write;
  int newline_write;

  // without arguments
  if(argc == 1 ){
    func();
  }
  // 1st file - 2nd file
  if(argc == 2 && argv[1][0] == '-'){
    func();
    return 0;
  }

    for (int p = 1; p < argc; p++)
      {
        char filename[100] = "==> ";
        strcat(filename, argv[p]);
        strcat(filename, " <==");
        strcat(filename, "\n");
        // Looking for "-"
        if(argv[p][0] == '-'){
          
            char new_line[1] = "\n";
            newline_write = write(STDOUT_FILENO, new_line, sizeof(new_line));
            if(newline_write == -1){
              perror("tail: error writing 'standard output'");
              return 1;
            }
            char filename[100] = "==> standard input <==\n";
            filename_write = write(STDOUT_FILENO, filename, strlen(filename));
            if(filename_write == -1){
              perror("tail: error writing 'standard output'");
              return 1;
            }
            func();
           
            char new_line2[1] = "\n";
            newline_write = write(STDOUT_FILENO, new_line2, sizeof(new_line2));
            if(newline_write == -1){
              perror("tail: error writing 'standard output'");
              return 1;
            }

            continue;
        }
      // open file and handle errors if any
      int fd = open (argv[p], O_RDONLY);
      if (fd == -1) {
              char fail_open[100] = "tail: cannot open '";
              strcat(fail_open, argv[p]);
              strcat(fail_open, "' for reading");
              perror(fail_open);
              if((p + 1) <= argc){
                  continue;
              }
              else
                  return 1;
          }
          if(argc != 2){
              filename_write = write(STDOUT_FILENO, filename, strlen(filename));
              if(filename_write == -1){
                res_close = close(fd);
                perror("tail: error writing 'standard output'");
                if(res_close == -1){
                  char fail_close[100] = "tail: error reading '";
                  strcat(fail_close, argv[p]);
                  strcat(fail_close, "'");
                  perror(fail_close);
                  return 1;
                }
                return 1;
              }    
          }
        
        // go -buf_size from the end of the file
        lseek(fd, -buf_size, SEEK_END);
        
        // read from file
        while ((read_res = read(fd, buf, buf_size)) != 0)
          {
            if (read_res == -1) {
                char fail_read[100] = "tail: error reading '";
                strcat(fail_read, argv[p]);
                strcat(fail_read, "'");
                perror(fail_read);
                break;
              }
              

            // take the read_res and bypass the buffer from back to front till finds 10 lines
            lines = 0;
              for(i = (read_res-1); i != -1; i--){
                  if(buf[i] == '\n'){
                      lines++;
                      if(lines == 11){
                        // p_buf to the 10th new line from back to front +1 to skip the new line
                        p_buf = &buf[i+1];
                        // total bites read - those we dont need to read (-1 for the new line char) 
                        how_much_to_read = (read_res - i) - 1; 
                        break; 
                      }     
                  }
              }
              if(lines < 10){
                // point to buffer
                p_buf = buf; 
                // write all read characters
                how_much_to_read = read_res; 
              }
           
            while ((write_res = write (STDOUT_FILENO, p_buf, how_much_to_read)) != 0) {
                if (write_res == -1) {
                    perror ("tail: error writing 'standard output'");
                    break;
                  }
                p_buf += write_res;
                how_much_to_read -= write_res;
              }
          }
          // new line only if there are 2 or more files for agrv
          // we dont need new emty line after the last file
          if ((p+1) != argc && argc > 2 && argv[p+1][0] != '-'){ 
            int fd2 = open(argv[p+1], O_RDONLY);
            if(fd2 == -1)
            {
              char fail_open[100] = "tail: cannot open '";
              strcat(fail_open, argv[p+1]);
              strcat(fail_open, "' for reading");
              perror(fail_open);
              break;
            }else{
              char new_line[1] = "\n";
              newline_write = write(STDOUT_FILENO, new_line, sizeof(new_line));
              if(newline_write == -1){
                perror("tail: error writing 'standard output'");
                res_close = close(fd2);
                if(res_close == -1){
                  char fail_close[100] = "tail: error reading '";
                  strcat(fail_close, argv[p]);
                  strcat(fail_close, "'");
                  perror(fail_close);
                  break;
                }
                break;
              }
              res_close = close(fd2);
              if(res_close == -1) {
                char fail_close[100] = "tail: error reading '";
                strcat(fail_close, argv[p]);
                strcat(fail_close, "'");
                perror(fail_close);
                return 1;
              }
            }
          }
      
        // close file and handle errors if any
        res_close = close (fd);
        if (res_close == -1)
          {
            char fail_close[100] = "tail: error reading '";
            strcat(fail_close, argv[p]);
            strcat(fail_close, "'");
            perror(fail_close);
            return 1;
          }
      }
  return 0;
}
