#include "inc/stdinc.h"
#include "inc/extvab.h"

// string split function
vector<string> split(const string& s, const string& delim)
{
	vector<string> elems;
	string tmp;
	size_t pos = 0;
	size_t len = s.length();
	size_t delim_len = delim.length();
	int find_pos;

	if (delim_len == 0) return elems;
	while (pos < len)
	{
		find_pos = s.find(delim, pos);
		if(find_pos < 0){
			elems.push_back(s.substr(pos, len - pos));
			break;
		}
		tmp = s.substr(pos, find_pos - pos);
		if(!tmp.empty()) elems.push_back(tmp);
		pos = find_pos + delim_len;
	}
	return elems;
}

bool isFileExist(const string &filename){
	bool flag = false;
	struct stat fileStat;
	if (stat(filename.c_str(), &fileStat) == 0)
		if(fileStat.st_size>0)
			flag = true;
	return flag;
}

// copy single file
int copySingleFile(const string &infilename, ofstream &outfile){
	ifstream infile;
	string line;

	if(isFileExist(infilename)==false) return 0;

	infile.open(infilename);
	if(!infile.is_open()){
		cerr << __func__ << ", line=" << __LINE__ << ": cannot open file:" << infilename << endl;
		exit(1);
	}
	// read each line and save to the output file
	while(getline(infile, line))
		if(line.size()>0 and line.at(0)!='#'){
			outfile << line << endl;
		}
	infile.close();

	return 0;
}

/**
 * Output the linked segments of queries.
 *  @return:
 *  	If succeeds, return SUCCESSFUL; otherwise return FAILED.
 */
short outputLinkedSegments(querySubject_t *pQuerySubject, int headRowSegmentLinkArray, int tailRowSegmentLinkArray, int itemNumSegmentLinkArray, segmentLink_t *segmentLinkArray, matchItem_t *matchItemArray)
{
	int headItemRow, headRow;
	matchItem_t *pMatchItemArray;

	pMatchItemArray = matchItemArray + pQuerySubject->firstRow;

	headRow = headRowSegmentLinkArray;
	while(headRow!=-1)
	{
		headItemRow = segmentLinkArray[headRow].arrRow;
		printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%lf\n", pMatchItemArray[headItemRow].startSubPos, pMatchItemArray[headItemRow].endSubPos, pMatchItemArray[headItemRow].startQueryPos, pMatchItemArray[headItemRow].endQueryPos, pMatchItemArray[headItemRow].strand, pMatchItemArray[headItemRow].matchLen, pMatchItemArray[headItemRow].totalMatchLen, pMatchItemArray[headItemRow].gapNum, pMatchItemArray[headItemRow].matchPercent);

		headRow = segmentLinkArray[headRow].next;
	}

	return SUCCESSFUL;
}

/**
 * Output the query match information to temporary text fasta file.
 * 	File format:
 * 		(1) Head: queryTitle, queryLen, subjectTitle, subjectLen, matchItemNum, matchKind, circularFlag;
 * 		(2) Body: startSubPos, endSubPos, startQueryPos, endQueryPos, strand, matchLen, totalMatchLen, gapNum, matchPercent.
 *
 *  @return:
 *  	If succeeds, return SUCCESSFUL; otherwise return FAILED.
 */
short outputQueryMatchInfoText(char *tmpQueryMatchFile, query_t *queryArray, matchItem_t *matchItemArray, int64_t itemNumQueryArray, subject_t *subjectArray)
{
	int64_t i, j, k, matchItemNum, subjectID, subjectLen, querySubjectNum;
	char *subjectTitle;
	FILE *fpTmpQueryMatch;
	matchItem_t *pTmpMatchItemArray;
	querySubject_t *pQuerySubjectArray;

	fpTmpQueryMatch = fopen(tmpQueryMatchFile, "w");
	if(fpTmpQueryMatch==NULL)
	{
		printf("In %s(), cannot open file [ %s ], error!\n", __func__, tmpQueryMatchFile);
		return FAILED;
	}

	// output all of the query match results
	for(i=0; i<itemNumQueryArray; i++)
	{
		querySubjectNum = queryArray[i].querySubjectNum;
		pQuerySubjectArray = queryArray[i].querySubArray;

		for(j=0; j<querySubjectNum; j++)
		{
			subjectID = pQuerySubjectArray[j].subjectID;
			subjectTitle = subjectArray[subjectID-1].subjectTitle;
			subjectLen = subjectArray[subjectID-1].subjectLen;
			matchItemNum = pQuerySubjectArray[j].matchItemNum;

			fprintf(fpTmpQueryMatch, ">%s\t%d\t%s\t%ld\t%ld\t%d\t%d\n", queryArray[i].queryTitle, queryArray[i].queryLen, subjectTitle, subjectLen, matchItemNum, pQuerySubjectArray[j].matchKind, pQuerySubjectArray[j].circularFlag);
			//fprintf(fpTmpQueryMatch, ">%s\t%d\t%s\t%ld\n", queryArray[i].queryTitle, queryArray[i].queryLen, subjectTitle, subjectLen);

			pTmpMatchItemArray = matchItemArray + pQuerySubjectArray[j].firstRow;
			for(k=0; k<matchItemNum; k++)
			{
				fprintf(fpTmpQueryMatch, "%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%.4f\n", pTmpMatchItemArray[k].startSubPos, pTmpMatchItemArray[k].endSubPos, pTmpMatchItemArray[k].startQueryPos, pTmpMatchItemArray[k].endQueryPos, pTmpMatchItemArray[k].strand, pTmpMatchItemArray[k].matchLen, pTmpMatchItemArray[k].totalMatchLen, pTmpMatchItemArray[k].gapNum, pTmpMatchItemArray[k].matchPercent);
			}
		}
	}


	fclose(fpTmpQueryMatch);
	fpTmpQueryMatch = NULL;

	return SUCCESSFUL;
}

/**
 * Check the reference covered ratio.
 *  @return:
 *  	If succeeds, return SUCCESSFUL; otherwise return FAILED.
 */
short checkRefCoveredRatio(metrics_t *queryMetrics, subject_t *subjectArray, int64_t itemNumSubjectArray, query_t *queryArray, int64_t itemNumQueryArray)
{
	int64_t i, j, k;
	validSegment_t *pValidSegArray;
	int32_t validSegmentNum;
	int64_t startRefPos, endRefPos, strand;
	uint8_t *pSubjectBitArray;
	int64_t subjectID, totalSegBaseNum, coveredBaseNum;
	int64_t totalErrorQueryNum, totalErrorQueryLen;

	totalErrorQueryNum = totalErrorQueryLen = 0;
	for(i=0; i<itemNumQueryArray; i++)
	{
		if(queryArray[i].queryLen>=minQueryLenThres)
		{
			if(queryArray[i].bestMatchRow>=0)
			{
				coveredBaseNum = 0;
				subjectID = queryArray[i].querySubArray[queryArray[i].bestMatchRow].subjectID;
				pValidSegArray = queryArray[i].querySubArray[queryArray[i].bestMatchRow].validSegArray;
				validSegmentNum = queryArray[i].querySubArray[queryArray[i].bestMatchRow].validSegmentNum;
				//validStrand = pValidSegArray[0].strand;
				for(j=0; j<validSegmentNum; j++)
				{
					pSubjectBitArray = queryMetrics->baseBitArrays[subjectID-1].subjectBitArray;
					totalSegBaseNum = queryMetrics->baseBitArrays[subjectID-1].totalRefBaseNum;

					strand = pValidSegArray[j].strand;
					startRefPos = pValidSegArray[j].startSubPos;
					endRefPos = pValidSegArray[j].endSubPos;

					if(strand==PLUS_STRAND)
					{
						if(startRefPos<1 || endRefPos>totalSegBaseNum)
						{
							printf("line=%d, In %s(), startRefPos=%ld, endRefPos=%ld > totalSegBaseNum=%ld, error!\n", __LINE__, __func__, startRefPos, endRefPos, totalSegBaseNum);
							return FAILED;
						}

						for(k=startRefPos; k<=endRefPos; k++)
						{
							if(pSubjectBitArray[k]>0)
								coveredBaseNum ++;
						}
					}else
					{
						if(startRefPos>totalSegBaseNum || endRefPos<1)
						{
							printf("line=%d, In %s(), startRefPos=%ld > totalSegBaseNum=%ld, endRefPos=%ld, error!\n", __LINE__, __func__, startRefPos, totalSegBaseNum, endRefPos);
							return FAILED;
						}

						for(k=startRefPos; k>=endRefPos; k--)
						{
							if(pSubjectBitArray[k]>0)
								coveredBaseNum ++;
						}
					}
				}

				if(coveredBaseNum<0.7*queryArray[i].queryLen)
				{
					totalErrorQueryLen += queryArray[i].queryLen;
					totalErrorQueryNum ++;

					printf("queryID=%d, queryTitle=%s, coveredBaseNum=%ld, queryLen=%d, globalMatchKind=%d, error!\n", queryArray[i].queryID, queryArray[i].queryTitle, coveredBaseNum, queryArray[i].queryLen, queryArray[i].globalMatchKind);
				}
			}
		}
	}

	printf("totalErrorQueryNum=%ld, totalErrorQueryLen=%ld\n", totalErrorQueryNum, totalErrorQueryLen);


	return SUCCESSFUL;
}


/**
 * Output read sequences in read set.
 *  @return:
 *  	If succeeds, return SUCCESSFUL; otherwise, return FAILED.
 */
short outputReadseqInReadset(char *outfile, readSet_t *readSet)
{
	int32_t i, j;
	readBlock_t *pReadBlockArray;
	read_t *pRead, *pReadArray;
	uint64_t *readseqInt;
	int64_t rid;
	char readBaseSeq[1000];
	FILE *fpOut;

	fpOut = fopen(outfile, "w");
	if(fpOut==NULL)
	{
		printf("line=%d, In %s(), cannot open file [ %s ], error!\n", __LINE__, __func__, outfile);
		return FAILED;
	}

	rid = 0;
	for(i=0; i<readSet->blocksNumRead; i++)
	{
		pReadBlockArray = readSet->readBlockArr + i;
		pReadArray = pReadBlockArray->readArr;
		for(j=0; j<(int32_t)pReadBlockArray->itemNum; j++)
		{
			rid ++;
			pRead = pReadArray + j;

			if(pRead->validFlag==YES)
			{
				readseqInt = readSet->readseqBlockArr[pRead->readseqBlockID-1].readseqArr + pRead->rowReadseqInBlock;
				if(getReadBaseByInt(readBaseSeq, readseqInt, pRead->seqlen)==FAILED)
				{
					printf("line=%d, In %s(), cannot get the read base sequence, error!\n", __LINE__, __func__);
					return FAILED;
				}
				fprintf(fpOut, "rid=%ld, seqlen=%d, nBaseNum=%d, validFlag=%d, successMapFlag=%d, uniqueMapFlag=%d, seq=%s\n", rid, (int32_t)pRead->seqlen, (int32_t)pRead->nBaseNum, (int32_t)pRead->validFlag, (int32_t)pRead->successMapFlag, (int32_t)pRead->uniqueMapFlag, readBaseSeq);
			}else
			{

				fprintf(fpOut, "rid=%ld, seqlen=%d, nBaseNum=%d, validFlag=%d, successMapFlag=%d, uniqueMapFlag=%d, seq=NNNN...NNNN\n", rid, (int32_t)pRead->seqlen, (int32_t)pRead->nBaseNum, (int32_t)pRead->validFlag, (int32_t)pRead->successMapFlag, (int32_t)pRead->uniqueMapFlag);
			}
		}
	}

	fclose(fpOut);
	fpOut = NULL;

	printf("congratulations for checking readSet!\n");

	return SUCCESSFUL;
}

/**
 * Output the gaps in queries.
 *  @return:
 *  	If succeeds, return SUCCESSFUL; otherwise, return FAILED.
 */
short outputGapRegInQueries(queryMatchInfo_t *queryMatchInfoSet)
{
	query_t *queryItem;
	int32_t i, j, queryLen, gapStartRow, gapEndRow, gapLen;
	char *querySeq, gapInfoFile[256];
	FILE *fpGapInfo;

	strcpy(gapInfoFile, outputPathStr);
	strcat(gapInfoFile, "gapInfo_query");

	fpGapInfo = fopen(gapInfoFile, "w");
	if(fpGapInfo==NULL)
	{
		printf("line=%d, In %s(), cannot open file [ %s ], error!\n", __LINE__, __func__, gapInfoFile);
		return FAILED;
	}

	for(i=0; i<queryMatchInfoSet->itemNumQueryArray; i++)
	{
		queryItem = queryMatchInfoSet->queryArray + i;
		querySeq = queryItem->querySeq;
		queryLen = queryItem->queryLen;

		gapStartRow = gapEndRow = -1;
		for(j=0; j<queryLen; j++)
		{
			if(querySeq[j]=='N' && gapStartRow==-1)
			{
				gapStartRow = j;
			}else if(querySeq[j]!='N' && gapStartRow>=0)
			{
				gapEndRow = j - 1;

				gapLen = gapEndRow - gapStartRow + 1;

				fprintf(fpGapInfo, "%s\t%d\t%d\t%d\n", queryItem->queryTitle, gapStartRow+1, gapEndRow+1, gapLen);

				gapStartRow = gapEndRow = -1;
			}
		}
	}

	fclose(fpGapInfo);

	return SUCCESSFUL;
}

/**
 * Output the gaps in queries.
 *  @return:
 *  	If succeeds, return SUCCESSFUL; otherwise, return FAILED.
 */
short outputMatchResults(alignMatchItem_t *matchResultArray, int32_t itemNum)
{
	int32_t i;
	char orientCh;

	if(itemNum>0)
	{
		printf("Match items: %d\n", itemNum);
		for(i=0; i<itemNum; i++)
		{
			if(matchResultArray[i].orientation==ORIENTATION_PLUS)
				orientCh = '+';
			else
				orientCh = '-';

			printf("queryID=%d, queryPos=%d, orient=%c, mismatchNum=%d, startReadPos=%d, alignSize=%d, fragSize=%d\n", matchResultArray[i].queryID, matchResultArray[i].queryPos, orientCh, (int32_t)matchResultArray[i].mismatchNum, (int32_t)matchResultArray[i].startReadPos, (int32_t)matchResultArray[i].alignSize, (int32_t)matchResultArray[i].fragSize);
		}
	}else
	{
		printf("There is no match item.\n");
	}

	return SUCCESSFUL;
}
