library(ggplot2)
library(rlist)
# raw = read.table("~/Documents/ENGI9875/lab2/pthreads.dat", skip = 1)
raw = read.table("~/Documents/ENGI9875/lab2/libdispatch.dat", skip = 1)

df = as.data.frame.matrix(raw) 
JOBS <- df$V1
WORK <- df$V2
Average <- df$V7

ggplot(df,aes(log(JOBS, 10), Average, group = WORK, color=WORK)) + geom_line()
ggplot(df,aes(log(WORK, 10), Average, group = JOBS, color=JOBS)) + geom_line()

Throughput <- vector(mode="numeric", length=0)
Number_of_pthreads <- vector(mode="numeric", length=0)
for(i in 1:dim(df)[1]){
  if(!i%%4){
    dem = Average[4]
  }
  else
    dem = Average[i%%4]
  Throughput <- c(Throughput, 1- Average[i]/dem) 
}

ggplot(df,aes(log(JOBS,10), Throughput, group = WORK, color=WORK)) + geom_line()
