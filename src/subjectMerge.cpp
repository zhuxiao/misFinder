#include "inc/stdinc.h"
#include "inc/extvab.h"


/**
 * Merge reference segments into a single file.
 *  @return:
 *  	If succeeds, return SUCCESSFUL; otherwise return FAILED.
 */
short mergeRefSegmentsFasta(char *mergedSegFile, char *subjectsFile)
{
	char **segFileArray;
	int32_t segFileNum;


	// initialize the memory for reference segments merge
	if(initMemRefSegmentsFasta(&segFileArray, &segFileNum, subjectsFile)==FAILED)
	{
		printf("line=%d, In %s(), cannot initialize the memory for reference segments merge, error!\n", __LINE__, __func__);
		return FAILED;
	}

	// merge reference segments
	if(mergeRefSegsFasta(mergedSegFile, segFileArray, segFileNum)==FAILED)
	{
		printf("line=%d, In %s(), cannot merge segments in fasta, error!\n", __LINE__, __func__);
		return FAILED;
	}

	// free memory for reference segments merging
	freeMemRefSegmentsFasta(segFileArray, segFileNum);

	return SUCCESSFUL;
}

/**
 * Initialize the memory for reference segments mergence.
 *  @return:
 *  	If succeeds, return SUCCESSFUL; otherwise return FAILED.
 */
short initMemRefSegmentsFasta(char ***segFileArray, int32_t *segFileNum, char *subjectsFile)
{
	FILE *fpSubjects;
	char line[LINE_CHAR_MAX+1];
	int fileStatus, len;
	int32_t tmpFileNum;

	// get the total segment number
	if(getSegmentFileNum(segFileNum, subjectsFile)==FAILED)
	{
		printf("line=%d, In %s(), cannot get the segment file number, error!\n", __LINE__, __func__);
		return FAILED;
	}
	if((*segFileNum)<=0)
	{
		printf("There are no segment files in [ %s ], please configure the segment files first.\n", subjectsFile);
		return FAILED;
	}

	// allocate the segment file memory
	*segFileArray = (char **) calloc (*segFileNum, sizeof(char*));
	if((*segFileArray)==NULL)
	{
		printf("line=%d, In %s(), cannot allocate memory, error!\n", __LINE__, __func__);
		return FAILED;
	}
	for(tmpFileNum=0; tmpFileNum<*segFileNum; tmpFileNum++)
	{
		(*segFileArray)[tmpFileNum] = (char *) calloc (LINE_CHAR_MAX+1, sizeof(char));
		if((*segFileArray)[tmpFileNum]==NULL)
		{
			printf("line=%d, In %s(), cannot allocate memory, error!\n", __LINE__, __func__);
			return FAILED;
		}
	}

	fpSubjects = fopen(subjectsFile, "r");
	if(fpSubjects==NULL)
	{
		printf("line=%d, In %s(), cannot open file [ %s ], error!\n", __LINE__, __func__, subjectsFile);
		return FAILED;
	}

	tmpFileNum = 0;
	while(1)
	{
		if(readLine(&fileStatus, line, &len, LINE_CHAR_MAX, fpSubjects)==FAILED)
		{
			printf("line=%d, In %s(), cannot read a line, error!\n", __LINE__, __func__);
			return FAILED;
		}

		if(len==0)
		{
			if(fileStatus==EOF_STATUS)
				break;
			else
				continue;
		}

		strcpy((*segFileArray)[tmpFileNum], line);
		tmpFileNum ++;
	}

	fclose(fpSubjects);
	fpSubjects = NULL;

	// ####################### Debug information #######################
	if(tmpFileNum!=(*segFileNum))
	{
		printf("line=%d, In %s(), tmpFileNum=%d !=segFileNum=%d, error!\n", __LINE__, __func__, tmpFileNum, *segFileNum);
		return FAILED;
	}
	// ####################### Debug information #######################

	return SUCCESSFUL;
}

/**
 * Release the memory of global parameters.
 */
void freeMemRefSegmentsFasta(char **segFileArray, int32_t segFileNum)
{
	int32_t i;

	for(i=0; i<segFileNum; i++)
	{
		free(segFileArray[i]);
		segFileArray[i] = NULL;
	}
	free(segFileArray);
	segFileArray = NULL;
}

/**
 * Get the subject segments file number in subjectsFile.
 *  @return:
 *  	If succeeds, return SUCCESSFUL; otherwise, return FAILED.
 */
short getSegmentFileNum(int32_t *segmentFileNum, const char *subjectsFileName)
{
	FILE *fpSubjects;
	char line[LINE_CHAR_MAX+1];
	int fileStatus, len;

	fpSubjects = fopen(subjectsFileName, "r");
	if(fpSubjects==NULL)
	{
		printf("line=%d, In %s(), cannot open file [ %s ], error!\n", __LINE__, __func__, subjectsFileName);
		return FAILED;
	}

	*segmentFileNum = 0;
	while(1)
	{
		if(readLine(&fileStatus, line, &len, LINE_CHAR_MAX, fpSubjects)==FAILED)
		{
			printf("In %s(), cannot read a line, error!\n", __func__);
			return FAILED;
		}

		if(len==0)
		{
			if(fileStatus==EOF_STATUS)
				break;
			else
				continue;
		}

		(*segmentFileNum) ++;
	}

	fclose(fpSubjects);
	fpSubjects = NULL;

	return SUCCESSFUL;
}

/**
 * Merge the segments in fasta.
 *  @return:
 *  	If succeeds, return SUCCESSFUL; otherwise, return FAILED.
 */
short mergeRefSegsFasta(const char *mergedSegFile, char **segFileArr, int64_t segFileNum)
{
	int64_t i;
	ofstream outfile_merged;

	outfile_merged.open(mergedSegFile);
	if(!outfile_merged.is_open()){
		cerr << __func__ << ", line=" << __LINE__ << ": cannot open file:" << mergedSegFile << endl;
		exit(1);
	}

	for(i=0; i<segFileNum; i++)
	{
		copySingleFile(segFileArr[i], outfile_merged);

//		if(bufLen>0)
//		{
//			// trim the tail non-base characters
//			if(filterTailNonBases(segBuf, &bufLen)==FAILED)
//			{
//				printf("line=%d, In %s(), cannot filter the tail non-base characters of segment buffer, error!\n", __LINE__, __func__);
//				free(segBuf); segBuf = NULL;
//				return FAILED;
//			}
//
//			fprintf(fpMergedSeg, "%s\n", segBuf);
//		}
	}

	outfile_merged.close();

	return SUCCESSFUL;
}

/**
 * Get the maximal file size.
 *  @return:
 *  	If succeeds, return SUCCESSFUL; otherwise, return FAILED.
 */
short getMaxFileSizeByte(int64_t *maxFileSizeByte, char **segFileArray, int64_t fileNums)
{
	int64_t i;
	struct stat st;

	*maxFileSizeByte = 0;
	for(i=0; i<fileNums; i++)
	{
		if(stat(segFileArray[i], &st)==-1)
		{
			printf("The file [ %s ] does not exist, please conform whether the file is correctly spelled!\n", segFileArray[i]);
			return FAILED;
		}

		if(st.st_size>(*maxFileSizeByte))
			*maxFileSizeByte = st.st_size;
	}

	if((*maxFileSizeByte)<=0)
	{
		printf("All the segments contain no sequence data, error!\n");
		return FAILED;
	}

	return SUCCESSFUL;
}

/**
 * Filter the tail non-base characters in the segment buffer.
 *  @return:
 *  	If succeeds, return SUCCESSFUL; otherwise, return FAILED.
 */
short filterTailNonBases(char *segBuf, int64_t *bufLen)
{
	int64_t i, tmpBufLen, validStartPos, validEndPos;

	tmpBufLen = *bufLen;

	// get the valid start character position
	i = 0;
	while(i<tmpBufLen)
	{
		if(segBuf[i]=='\n')
		{
			validStartPos = i + 1;
			break;
		}
		i++;
	}

	validEndPos = -1;
	for(i=tmpBufLen-1; i>=validStartPos; i--)
	{
		switch(segBuf[i])
		{
			case 'A':
			case 'C':
			case 'G':
			case 'T':
			case 'a':
			case 'c':
			case 'g':
			case 't':
			case 'N':
			case 'n':
			case '.':
				validEndPos = i;
				break;
		}

		if(validEndPos>=0)
			break;
	}

	if(validEndPos<validStartPos)
	{
		printf("line=%d, In %s(), validEndPos=%ld < validStartPos=%ld, error!\n", __LINE__, __func__, validEndPos, validStartPos);
		return FAILED;
	}

	segBuf[validEndPos+1] = '\0';
	*bufLen = validEndPos + 1;

	return SUCCESSFUL;
}

/**
 * fill the segment buffer.
 *  @return:
 *  	If succeeds, return SUCCESSFUL; otherwise, return FAILED.
 */
short fillSegBuf(char *segBuf, int64_t *bufLen, char *segmentFile)
{
	FILE *fpSegment;
	char ch;

	fpSegment = fopen(segmentFile, "r");
	if(fpSegment==NULL)
	{
		printf("line=%d, In %s(), cannot open file [ %s ], error!\n", __LINE__, __func__, segmentFile);
		return FAILED;
	}

	*bufLen = 0;
	while((ch=fgetc(fpSegment))!=EOF) segBuf[(*bufLen)++] = ch;

	fclose(fpSegment);
	fpSegment = NULL;

	return SUCCESSFUL;
}

/**
 * Fill subject sequences.
 *  @return:
 *  	If succeeds, return SUCCESSFUL; otherwise, return FAILED.
 */
short fillSubjectSeqs(queryMatchInfo_t *queryMatchInfoSet, char *mergedSegFile)
{
	int64_t i, maxSubjectLen, subjectID, subjectLen, returnFlag;
	subject_t *subjectArray;
	char *subjectSeq, subjectHeadTitle[1000], *pch;
	FILE *fpSubject;

	fpSubject = fopen(mergedSegFile, "r");
	if(fpSubject==NULL)
	{
		printf("line=%d, In %s(), cannot open file [ %s ], error!\n", __LINE__, __func__, mergedSegFile);
		return FAILED;
	}

	subjectArray = queryMatchInfoSet->subjectArray;

	maxSubjectLen = 0;
	for(i=0; i<queryMatchInfoSet->itemNumSubjectArray; i++)
		if(maxSubjectLen<subjectArray[i].subjectLen)
			maxSubjectLen = subjectArray[i].subjectLen;

	subjectSeq = (char *) malloc ((maxSubjectLen+1)*sizeof(char));
	if(subjectSeq==NULL)
	{
		printf("line=%d, In %s(), cannot allocate memory, error!\n", __LINE__, __func__);
		return FAILED;
	}

	subjectID = 1;
	while((returnFlag=getSingleFastaItemFromFile(fpSubject, subjectHeadTitle, subjectSeq, &subjectLen))==SUCCESSFUL)
	{
		// trim head title after space or tab character
		pch = subjectHeadTitle;
		while(*pch)
		{
			if((*pch)==' ' || (*pch)=='\t')
			{
				*pch = '\0';
				break;
			}
			pch ++;
		}

		if(strcmp(subjectHeadTitle, subjectArray[subjectID-1].subjectTitle)==0)
		{
			subjectArray[subjectID-1].subjectSeq = (char*) calloc (subjectArray[subjectID-1].subjectLen+1, sizeof(char));
			if(subjectArray[subjectID-1].subjectSeq==NULL)
			{
				printf("line=%d, In %s(), cannot allocate memory, error!\n", __LINE__, __func__);
				return FAILED;
			}
			strcpy(subjectArray[subjectID-1].subjectSeq, subjectSeq);
		}else
		{
			printf("line=%d, In %s(), cannot get the subject base sequences, error!\n", __LINE__, __func__);
			return FAILED;
		}

		subjectID ++;
	}


	free(subjectSeq);
	subjectSeq = NULL;

	fclose(fpSubject);

	return SUCCESSFUL;
}
