# Hardcoded omdat daarom

comparison <- read.csv("comparison.csv", header=TRUE)

###
name <- "armadillo"
index <- 1
counts_matrix <- matrix(c(comparison[index,]$vertexface, comparison[index,]$edgeedge, comparison[index,]$cpuvertexface, comparison[index,]$cpuedgeedge),ncol=2,byrow=TRUE)
colnames(counts_matrix) <- c("vertex-face", "edge-edge")
rownames(counts_matrix) <- c("GPU", "CPU")
counts <- as.table(counts_matrix)

pdf(paste(name, ".pdf", sep=""))
barplot(counts, xlab="Collisions left", col=c("darkblue", "red"), legend = rownames(counts), beside=TRUE, args.legend=list(x=2))
dev.off()
###

###
name <- "asiandragon"
index <- 2
counts_matrix <- matrix(c(comparison[index,]$vertexface, comparison[index,]$edgeedge, comparison[index,]$cpuvertexface, comparison[index,]$cpuedgeedge),ncol=2,byrow=TRUE)
colnames(counts_matrix) <- c("vertex-face", "edge-edge")
rownames(counts_matrix) <- c("GPU", "CPU")
counts <- as.table(counts_matrix)

pdf(paste(name, ".pdf", sep=""))
barplot(counts, xlab="Collisions left", col=c("darkblue", "red"), legend = rownames(counts), beside=TRUE, args.legend=list(x=2))
dev.off()
###

###
name <- "bunny2"
index <- 3
counts_matrix <- matrix(c(comparison[index,]$vertexface, comparison[index,]$edgeedge, comparison[index,]$cpuvertexface, comparison[index,]$cpuedgeedge),ncol=2,byrow=TRUE)
colnames(counts_matrix) <- c("vertex-face", "edge-edge")
rownames(counts_matrix) <- c("GPU", "CPU")
counts <- as.table(counts_matrix)

pdf(paste(name, ".pdf", sep=""))
barplot(counts, xlab="Collisions left", col=c("darkblue", "red"), legend = rownames(counts), beside=TRUE, main=paste("Comparison for", name))
dev.off()
###

###
name <- "bunny"
index <- 4
counts_matrix <- matrix(c(comparison[index,]$vertexface, comparison[index,]$edgeedge, comparison[index,]$cpuvertexface, comparison[index,]$cpuedgeedge),ncol=2,byrow=TRUE)
colnames(counts_matrix) <- c("vertex-face", "edge-edge")
rownames(counts_matrix) <- c("GPU", "CPU")
counts <- as.table(counts_matrix)

pdf(paste(name, ".pdf", sep=""))
barplot(counts, xlab="Collisions left", col=c("darkblue", "red"), legend = rownames(counts), beside=TRUE, args.legend=list(x=2))
dev.off()
###

###
name <- "lucy"
index <- 5
counts_matrix <- matrix(c(comparison[index,]$vertexface, comparison[index,]$edgeedge, comparison[index,]$cpuvertexface, comparison[index,]$cpuedgeedge),ncol=2,byrow=TRUE)
colnames(counts_matrix) <- c("vertex-face", "edge-edge")
rownames(counts_matrix) <- c("GPU", "CPU")
counts <- as.table(counts_matrix)

pdf(paste(name, ".pdf", sep=""))
barplot(counts, xlab="Collisions left", col=c("darkblue", "red"), legend = rownames(counts), beside=TRUE, args.legend=list(x=2))
dev.off()
###

###
name <- "neptune"
index <- 6
counts_matrix <- matrix(c(comparison[index,]$vertexface, comparison[index,]$edgeedge, comparison[index,]$cpuvertexface, comparison[index,]$cpuedgeedge),ncol=2,byrow=TRUE)
colnames(counts_matrix) <- c("vertex-face", "edge-edge")
rownames(counts_matrix) <- c("GPU", "CPU")
counts <- as.table(counts_matrix)

pdf(paste(name, ".pdf", sep=""))
barplot(counts, xlab="Collisions left", col=c("darkblue", "red"), legend = rownames(counts), beside=TRUE, args.legend=list(x=2))
dev.off()
###

###
name <- "sphere"
index <- 7
counts_matrix <- matrix(c(comparison[index,]$vertexface, comparison[index,]$edgeedge, comparison[index,]$cpuvertexface, comparison[index,]$cpuedgeedge),ncol=2,byrow=TRUE)
colnames(counts_matrix) <- c("vertex-face", "edge-edge")
rownames(counts_matrix) <- c("GPU", "CPU")
counts <- as.table(counts_matrix)

pdf(paste(name, ".pdf", sep=""))
barplot(counts, xlab="Collisions left", col=c("darkblue", "red"), legend = rownames(counts), beside=TRUE, args.legend=list(x=2))
dev.off()
###