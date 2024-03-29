#include "inc/stdinc.h"
#include "inc/extvab.h"


/**
 * Compute the structural variations in queries.
 *  @return:
 *  	If succeeds, return SUCCESSFUL; otherwise, return FAILED.
 */
short computeSVInQueries(queryMatchInfo_t *queryMatchInfoSet, readSetArr_t *readSetArray, int32_t threadNum)
{
	struct timeval tpstart, tpend;
	double timeused;
	gettimeofday(&tpstart, NULL);

	// initialize the data for threads
	if(initThreadParasSV(&threadArr, &threadParaArr, threadNum, queryMatchInfoSet, readSetArray->readSetArray)==FAILED)
	{
		printf("line=%d, In %s(), cannot initialize mis-assembly identification threads, error!\n", __LINE__, __func__);
		return FAILED;
	}

	// start multiple threads
	if(createThreadsSV(threadArr, threadParaArr, threadNum)==FAILED)
	{
		printf("line=%d, In %s(), cannot create threads, error.\n", __LINE__, __func__);
		return FAILED;
	}

	if(waitThreads(threadArr, threadParaArr, threadNum)==FAILED)
	{
		printf("line=%d, In %s(), cannot wait threads, error.\n", __LINE__, __func__);
		return FAILED;
	}

	// free memory for thread parameters
	if(freeThreadParasMisass(&threadArr, &threadParaArr, threadNum)==FAILED)
	{
		printf("line=%d, In %s(), cannot free the thread parameters, error.\n", __LINE__, __func__);
		return FAILED;
	}

	// calculate running time
	gettimeofday(&tpend, NULL);
	timeused = tpend.tv_sec-tpstart.tv_sec+ (double)(tpend.tv_usec-tpstart.tv_usec)/1000000;

	printf("Correct assembly identification used time: %.2f seconds.\n", timeused);

	return SUCCESSFUL;
}

/**
 * Initialize thread parameters for correct assembly validation.
 *  @return:
 *  	If succeeds, return SUCCESSFUL; otherwise, return FAILED.
 */
short initThreadParasSV(pthread_t **threadArray, threadPara_t **threadParaArray, int32_t threadNum, queryMatchInfo_t *queryMatchInfoSet, readSet_t *readSet)
{
	int32_t i, validThreadNum;

	*threadArray = (pthread_t*) calloc (threadNum, sizeof(pthread_t));
	if((*threadArray)==NULL)
	{
		printf("line=%d, In %s(), cannot allocate memory, error.\n", __LINE__, __func__);
		return FAILED;
	}

	*threadParaArray = (threadPara_t*) calloc (threadNum, sizeof(threadPara_t));
	if((*threadParaArray)==NULL)
	{
		printf("line=%d, In %s(), cannot allocate memory, error.\n", __LINE__, __func__);
		return FAILED;
	}

	//queryNum = queryMatchInfoSet->itemNumQueryArray;
	if(threadNum<queryMatchInfoSet->itemNumQueryArray)
		validThreadNum = threadNum;
	else
		validThreadNum = queryMatchInfoSet->itemNumQueryArray;

	for(i=0; i<threadNum; i++)
	{
		(*threadParaArray)[i].threadID = i;
		(*threadParaArray)[i].validThreadNum = validThreadNum;
		(*threadParaArray)[i].successFlag = NO;

		if(i<validThreadNum)
		{
			(*threadParaArray)[i].queryMatchInfoSet = queryMatchInfoSet;
			(*threadParaArray)[i].readSet = readSet;
			(*threadParaArray)[i].validFlag = YES;
		}else
		{
			(*threadParaArray)[i].validFlag = NO;
		}
	}

	return SUCCESSFUL;
}

/**
 * Create the threads for mis-assembly identification.
 *  @return:
 *  	If succeeds, return SUCCESSFUL; otherwise return FAILED.
 */
short createThreadsSV(pthread_t *threadArray, threadPara_t *threadParaArray, int32_t threadNum)
{
	int32_t i, ret, validNum;

	validNum = 0;
	for(i=0; i<threadNum; i++)
		if(threadParaArray[i].validFlag==YES)
			validNum ++;

	printf("Begin identifying correct assemblies due to structural variations using %d threads, please wait ...\n", validNum);

	for(i=0; i<threadNum; i++)
	{
		if(threadParaArray[i].validFlag==YES)
		{
			ret = pthread_create(threadArray+i, NULL, computeSVInQueriesSingleThread, threadParaArray+i);
			if(ret!=0)
			{
				printf("line=%d, In %s(), cannot create threads for mis-assembly identification, error!\n", __LINE__, __func__);
				return FAILED;
			}
		}
	}

	return SUCCESSFUL;
}

/**
 * Compute the structural variations in queries.
 *  @return:
 *  	If succeeds, return SUCCESSFUL; otherwise, return FAILED.
 */
void* computeSVInQueriesSingleThread(void *arg)
{
	threadPara_t *threadPara = (threadPara_t *)arg;
	int32_t i, threadID, validThreadNum, processedNum;
	query_t *queryArray;
	queryMatchInfo_t *queryMatchInfoSet;
	readSet_t *readSet;

	threadID = threadPara->threadID;
	validThreadNum = threadPara->validThreadNum;

	queryMatchInfoSet = threadPara->queryMatchInfoSet;
	queryArray = queryMatchInfoSet->queryArray;
	readSet = threadPara->readSet;

	processedNum = 0;
	queryArray = queryMatchInfoSet->queryArray;
	for(i=0; i<queryMatchInfoSet->itemNumQueryArray; i++)
	{
		if(i%validThreadNum==threadID && (queryArray[i].misassFlag==UNCERTAIN_MISASS || queryArray[i].misassFlag==TRUE_MISASS))
		{
			// ########################### Debug information ##############################
			//if(queryArray[i].queryID==21900 || strcmp(queryArray[i].queryTitle, "scf7180000614599")==0)
			//{
			//	printf("------ queryID=%d, queryTitle=%s, queryLen=%d, subjectNum=%d\n", queryArray[i].queryID, queryArray[i].queryTitle, queryArray[i].queryLen, queryArray[i].querySubjectNum);
			//}
			// ########################### Debug information ##############################

			// compute the structural variations in single query
			if(computeSVInSingleQuery(queryArray+i, queryMatchInfoSet->subjectArray, readSet)==FAILED)
			{
				printf("line=%d, In %s(), cannot determine structural variations for single query, error!\n", __LINE__, __func__);
				return NULL;
			}
		}

		processedNum ++;
//		if(i%validThreadNum==threadID && processedNum%100==0)
//			printf("Queries processed: %d\n", processedNum);
	}

//	if(threadID==validThreadNum-1 && processedNum%100!=0)
//		printf("Queries processed: %d\n", processedNum);

	return NULL;
}

/**
 * Compute the structural variations in single query.
 *  @return:
 *  	If succeeds, return SUCCESSFUL; otherwise, return FAILED.
 */
short computeSVInSingleQuery(query_t *queryItem, subject_t *subjectArray, readSet_t *readSet)
{
	int32_t queryLen;
	baseCov_t *baseCovArray;

	queryLen = queryItem->queryLen;
	baseCovArray = (baseCov_t *) calloc (queryLen, sizeof(baseCov_t));
	if(baseCovArray==NULL)
	{
		printf("line=%d, In %s(), cannot allocate memory, error!\n", __LINE__, __func__);
		return FAILED;
	}

	// compute base coverage
	if(computeBaseCovSingleQuery(baseCovArray, queryItem, readSet)==FAILED)
	{
		printf("line=%d, In %s(), cannot compute the base coverage of single query, error!\n", __LINE__, __func__);
		return FAILED;
	}

	if(determineSVInSingleQuery(queryItem, subjectArray, baseCovArray, readSet)==FAILED)
	{
		printf("line=%d, In %s(), cannot determine the base coverage of single query, error!\n", __LINE__, __func__);
		return FAILED;
	}

	free(baseCovArray);

	return SUCCESSFUL;
}

/**
 * Determine the structural variation region for single query.
 *  @return:
 *  	If succeeds, return SUCCESSFUL; otherwise, return FAILED.
 */
short determineSVInSingleQuery(query_t *queryItem, subject_t *subjectArray, baseCov_t *baseCovArray, readSet_t *readSet)
{
	// determine SV in misjoin region
	if(determineSVMisjoinReg(queryItem, subjectArray, baseCovArray, readSet)==FAILED)
	{
		printf("line=%d, In %s(), cannot check the structural variation region of indel region, error!\n", __LINE__, __func__);
		return FAILED;
	}

	// determine SV in indel region
	if(determineSVIndelReg(queryItem, subjectArray, baseCovArray, readSet)==FAILED)
	{
		printf("line=%d, In %s(), cannot check the structural variation region of indel region, error!\n", __LINE__, __func__);
		return FAILED;
	}

	return SUCCESSFUL;
}

/**
 * Determine SV in misjoin region.
 *  @return:
 *  	If succeeds, return SUCCESSFUL; otherwise, return FAILED.
 */
short determineSVMisjoinReg(query_t *queryItem, subject_t *subjectArray, baseCov_t *baseCovArray, readSet_t *readSet)
{
	misInfo_t *misInfo;
	queryMargin_t *queryMargin;

	misInfo = queryItem->misInfoList;
	while(misInfo)
	{
		if(misInfo->misType==QUERY_MISJOIN_KIND)
		{
			queryMargin = misInfo->queryMargin;
			if(queryMargin->misassFlag==UNCERTAIN_MISASS)
			{
				// check the structural variation region
				if(checkSVRegMisjoin(queryMargin, baseCovArray, queryItem, readSet)==FAILED)
				{
					printf("line=%d, In %s(), cannot check the structural variation region of indel region, error!\n", __LINE__, __func__);
					return FAILED;
				}
				misInfo->misassFlag = queryMargin->misassFlag;
			}
		}
		misInfo = misInfo->next;
	}

	return SUCCESSFUL;
}

/**
 * Determine SV in indel region.
 *  @return:
 *  	If succeeds, return SUCCESSFUL; otherwise, return FAILED.
 */
short determineSVIndelReg(query_t *queryItem, subject_t *subjectArray, baseCov_t *baseCovArray, readSet_t *readSet)
{
	misInfo_t *misInfo;
	queryIndel_t *queryIndel;

	misInfo = queryItem->misInfoList;
	while(misInfo)
	{
		if(misInfo->misType==QUERY_INDEL_KIND)
		{
			queryIndel = misInfo->queryIndel;
			if(queryIndel->misassFlag==UNCERTAIN_MISASS)
			{
				// check the structural variation region
				if(checkSVRegQueryIndel(queryIndel, baseCovArray, queryItem, readSet)==FAILED)
				{
					printf("line=%d, In %s(), cannot check the structural variation region of indel region, error!\n", __LINE__, __func__);
					return FAILED;
				}
				misInfo->misassFlag = queryIndel->misassFlag;
			}
		}
		misInfo = misInfo->next;
	}

	return SUCCESSFUL;
}

/**
 * Check the structural variation region in misjoin region.
 *  @return:
 *  	If succeeds, return SUCCESSFUL; otherwise, return FAILED.
 */
short checkSVRegMisjoin(queryMargin_t *queryMargin, baseCov_t *baseCovArray, query_t *queryItem, readSet_t *readSet)
{
	int32_t startRegPos, endRegPos, subRegSize, ratioRegionNum, leftPos, rightPos;
	ratioRegion_t *ratioRegionArray;

	subRegSize = 500;

	if(queryMargin->leftMargin<queryMargin->rightMargin)
	{
		leftPos = queryMargin->leftMargin;
		rightPos = queryMargin->rightMargin;
	}else
	{
		leftPos = queryMargin->rightMargin;
		rightPos = queryMargin->leftMargin;
	}

	// get the region
	startRegPos = leftPos - 300;
	endRegPos = rightPos + 300;
	if(startRegPos<1)
		startRegPos = 1;
	if(endRegPos>queryItem->queryLen)
		endRegPos = queryItem->queryLen;

	// divide the region into sub-regions
	if(initRatioRegQueryIndel(&ratioRegionArray, &ratioRegionNum, startRegPos, endRegPos, subRegSize)==FAILED)
	{
		printf("line=%d, In %s(), cannot initialize the ratio sub-regions for indel region, error!\n", __LINE__, __func__);
		return FAILED;
	}

	// compute disagreements, discordantNum, insert size for each sub-region
	if(fillRatioRegionArray(ratioRegionArray, ratioRegionNum, queryItem, readSet, NO)==FAILED)
	{
		printf("line=%d, In %s(), cannot fill the ratioRegion array, error!\n", __LINE__, __func__);
		return FAILED;
	}

	// compute the disagreements of ratio regions
	if(computeDisagreeNumRatioRegs(ratioRegionArray, ratioRegionNum, baseCovArray, NO)==FAILED)
	{
		printf("line=%d, In %s(), cannot compute disagreements for the ratioRegion array, error!\n", __LINE__, __func__);
		return FAILED;
	}

	// compute region ratios
	if(computeRatios(ratioRegionArray, ratioRegionNum)==FAILED)
	{
		printf("line=%d, In %s(), cannot compute ratios for the ratioRegion array, error!\n", __LINE__, __func__);
		return FAILED;
	}

	// ###################### Debug information #########################
	//outputRatioRegionArray(ratioRegionArray, ratioRegionNum);
	// ###################### Debug information #########################

	// determine the SV for misjoin region
	if(determineSVMisjoin(queryMargin, ratioRegionArray, ratioRegionNum, subRegSize, queryItem)==FAILED)
	{
		printf("line=%d, In %s(), cannot determine the structural variation for query indel, error!\n", __LINE__, __func__);
		return FAILED;
	}

	if(ratioRegionNum>0)
		free(ratioRegionArray);

	return SUCCESSFUL;
}

/**
 * Check the structural variation region in indel region.
 *  @return:
 *  	If succeeds, return SUCCESSFUL; otherwise, return FAILED.
 */
short checkSVRegQueryIndel(queryIndel_t *queryIndel, baseCov_t *baseCovArray, query_t *queryItem, readSet_t *readSet)
{
	int32_t leftSegRow, rightSegRow, startRegPos, endRegPos, subRegSize, ratioRegionNum, leftPos, rightPos;
	ratioRegion_t *ratioRegionArray;

	subRegSize = 500;
	leftSegRow = queryIndel->leftSegRow;
	rightSegRow = queryIndel->rightSegRow;

	if(queryIndel->leftMargin<queryIndel->rightMargin)
	{
		leftPos = queryIndel->leftMargin;
		rightPos = queryIndel->rightMargin;
	}else
	{
		leftPos = queryIndel->rightMargin;
		rightPos = queryIndel->leftMargin;
	}

	if(leftSegRow==-1 && rightSegRow>=0)
	{ // head
		// get the region
		startRegPos = 1;
		endRegPos = rightPos + 200;
		if(endRegPos>queryItem->queryLen)
			endRegPos = queryItem->queryLen;
	}else if(leftSegRow>=0 && rightSegRow==-1)
	{ // tail
		// get the region
		startRegPos = leftPos - 200;
		endRegPos = queryItem->queryLen;
		if(startRegPos<1)
			startRegPos = 1;
	}else if(leftSegRow>=0 && rightSegRow>=0)
	{ // mid
		// get the region
		startRegPos = leftPos - 200;
		endRegPos = rightPos + 200;
		if(startRegPos<1)
			startRegPos = 1;
		if(endRegPos>queryItem->queryLen)
			endRegPos = queryItem->queryLen;
	}else
	{
		startRegPos = leftPos;
		endRegPos = rightPos;
	}

	// divide the region into sub-regions
	if(initRatioRegQueryIndel(&ratioRegionArray, &ratioRegionNum, startRegPos, endRegPos, subRegSize)==FAILED)
	{
		printf("line=%d, In %s(), cannot initialize the ratio sub-regions for indel region, error!\n", __LINE__, __func__);
		return FAILED;
	}

	// compute disagreements, insert size for each sub-region
	if(fillRatioRegionArray(ratioRegionArray, ratioRegionNum, queryItem, readSet, NO)==FAILED)
	{
		printf("line=%d, In %s(), cannot fill the ratioRegion array, error!\n", __LINE__, __func__);
		return FAILED;
	}

	// compute the disagreements of ratio regions
	if(computeDisagreeNumRatioRegs(ratioRegionArray, ratioRegionNum, baseCovArray, NO)==FAILED)
	{
		printf("line=%d, In %s(), cannot compute disagreements for the ratioRegion array, error!\n", __LINE__, __func__);
		return FAILED;
	}

	// compute region ratios
	if(computeRatios(ratioRegionArray, ratioRegionNum)==FAILED)
	{
		printf("line=%d, In %s(), cannot compute ratios for the ratioRegion array, error!\n", __LINE__, __func__);
		return FAILED;
	}

	// ###################### Debug information #########################
	//outputRatioRegionArray(ratioRegionArray, ratioRegionNum);
	// ###################### Debug information #########################

	// determine the SV for query indel
	if(determineSVQueryIndel(queryIndel, ratioRegionArray, ratioRegionNum, subRegSize, queryItem, readSet)==FAILED)
	{
		printf("line=%d, In %s(), cannot determine the structural variation for query indel, error!\n", __LINE__, __func__);
		return FAILED;
	}

	// free memory
	if(ratioRegionNum>0)
		free(ratioRegionArray);
	else
		printf("ratioRegionNum=%d\n", ratioRegionNum);

	return SUCCESSFUL;
}

/**
 * Divide the region into sub-regions for queryIndel node.
 *  @return:
 *  	If succeeds, return SUCCESSFUL; otherwise, return FAILED.
 */
short initRatioRegQueryIndel(ratioRegion_t **ratioRegionArray, int32_t *ratioRegionNum, int32_t startQueryPos, int32_t endQueryPos, int32_t subRegSize)
{
	int32_t i, subRegSizeNew, itemNum, startPos, endPos, midPos;

	*ratioRegionNum = (endQueryPos - startQueryPos) / subRegSize + 1;
	*ratioRegionArray = (ratioRegion_t*) calloc (*ratioRegionNum, sizeof(ratioRegion_t));
	if((*ratioRegionArray)==NULL)
	{
		printf("line=%d, In %s(), cannot allocate memory, error!\n", __LINE__, __func__);
		return FAILED;
	}
	subRegSizeNew = (endQueryPos - startQueryPos + 1) / (*ratioRegionNum) + 1;

	// initialize the positions
	itemNum = 0;
	for(i=0; i<(*ratioRegionNum); i++)
	{
		startPos = startQueryPos + i * subRegSizeNew;
		endPos = startPos + subRegSizeNew - 1;
		if(endPos>endQueryPos)
			endPos = endQueryPos;

		midPos = (startPos + endPos) / 2;
		(*ratioRegionArray)[itemNum].midQPos = midPos;
		(*ratioRegionArray)[itemNum].startQPosLHalf = startPos;
		(*ratioRegionArray)[itemNum].endQPosLHalf = midPos;
		(*ratioRegionArray)[itemNum].startQPosRHalf = midPos + 1;
		(*ratioRegionArray)[itemNum].endQPosRHalf = endPos;

		(*ratioRegionArray)[itemNum].disagreeNum = 0;
		(*ratioRegionArray)[itemNum].zeroCovNum = 0;
		itemNum ++;
	}

	if((*ratioRegionNum)!=itemNum)
	{
		printf("line=%d, In %s(), cannot ratioRegionNum=%d, itemNum=%d, error!\n", __LINE__, __func__, *ratioRegionNum, itemNum);
		return FAILED;
	}

	return SUCCESSFUL;
}

/**
 * Compute the disagreements of ratio regions.
 *  @return:
 *  	If succeeds, return SUCCESSFUL; otherwise, return FAILED.
 */
short computeDisagreeNumRatioRegs(ratioRegion_t *ratioRegionArray, int32_t ratioRegionNum, baseCov_t *baseCovArray, int32_t printFlag)
{
	int32_t i, startRow, endRow, disagreeRegSize;

	// compute the disagreements
	for(i=0; i<ratioRegionNum; i++)
	{
		if(ratioRegionArray[i].startQPosLHalf<ratioRegionArray[i].startQPosRHalf)
			startRow = ratioRegionArray[i].startQPosLHalf - 1;
		else
			startRow = ratioRegionArray[i].startQPosRHalf - 1;

		if(ratioRegionArray[i].endQPosLHalf<ratioRegionArray[i].endQPosRHalf)
			endRow = ratioRegionArray[i].endQPosRHalf - 1;
		else
			endRow = ratioRegionArray[i].endQPosLHalf - 1;

		if(computeDisagreements(&ratioRegionArray[i].disagreeNum, &ratioRegionArray[i].zeroCovNum, &disagreeRegSize, baseCovArray, startRow, endRow, printFlag)==FAILED)
		{
			printf("line=%d, In %s(), cannot compute the disagreements, error!\n", __LINE__, __func__);
			return FAILED;
		}
	}

	return SUCCESSFUL;
}

/**
 * Determine the SV for misjoin region.
 *  @return:
 *  	If succeeds, return SUCCESSFUL; otherwise, return FAILED.
 */
short determineSVMisjoin(queryMargin_t *queryMargin, ratioRegion_t *ratioRegionArray, int32_t ratioRegionNum, int32_t subRegSize, query_t *queryItem)
{
	int32_t totalDisagreeNum, totalZeroCovNum, discordantRegNum, headSkipRegNum, tailSkipRegNum;

	headSkipRegNum = 0;
	tailSkipRegNum = 0;

	// compute total disagreements and total zero coverage
	if(computeTotalDisagreeNum(&totalDisagreeNum, &totalZeroCovNum, ratioRegionArray, ratioRegionNum, headSkipRegNum, tailSkipRegNum)==FAILED)
	{
		printf("line=%d, In %s(), cannot compute the total disagreements, error!\n", __LINE__, __func__);
		return FAILED;
	}

	// get discordant region count
	if(getDiscordantRegNum(&discordantRegNum, ratioRegionArray, ratioRegionNum, headSkipRegNum, tailSkipRegNum)==FAILED)
	{
		printf("line=%d, In %s(), cannot compute the discordant region count, error!\n", __LINE__, __func__);
		return FAILED;
	}

	if(totalDisagreeNum>=3 || totalZeroCovNum>0 || discordantRegNum>=1)
		queryMargin->misassFlag = UNCERTAIN_MISASS;
	else
		queryMargin->misassFlag = STRUCTURE_VARIATION;

	//printf("misjoin reg[%d, %d]: misassFlag=%d, totalDisagreeNum=%d, totalZeroCovNum=%d, discordantRegNum=%d\n", queryMargin->leftMargin, queryMargin->rightMargin, queryMargin->misassFlag, totalDisagreeNum, totalZeroCovNum, discordantRegNum);

	return SUCCESSFUL;
}

/**
 * Determine the SV for query indel.
 *  @return:
 *  	If succeeds, return SUCCESSFUL; otherwise, return FAILED.
 */
short determineSVQueryIndel(queryIndel_t *queryIndel, ratioRegion_t *ratioRegionArray, int32_t ratioRegionNum, int32_t subRegSize, query_t *queryItem, readSet_t *readSet)
{
	int32_t totalDisagreeNum, totalZeroCovNum, discordantRegNum, headSkipRegNum, tailSkipRegNum;
	int32_t startQueryPosLeft, endQueryPosLeft, startQueryPosRight, endQueryPosRight;
	double insertSize, standDev, difFragSize;

	if(queryIndel->leftSegRow==-1)
		headSkipRegNum = 1;
	else
	{
		if(ratioRegionArray[0].startQPosRHalf<subRegSize)
			headSkipRegNum = 1;
		else
			headSkipRegNum = 0;
	}

	if(queryIndel->rightSegRow==-1)
		tailSkipRegNum = 1;
	else
	{
		if(ratioRegionArray[ratioRegionNum-1].endQPosLHalf>queryItem->queryLen-subRegSize)
			tailSkipRegNum = 1;
		else
			tailSkipRegNum = 0;
	}

	// compute total disagreements and total zero coverage
	if(computeTotalDisagreeNum(&totalDisagreeNum, &totalZeroCovNum, ratioRegionArray, ratioRegionNum, headSkipRegNum, tailSkipRegNum)==FAILED)
	{
		printf("line=%d, In %s(), cannot compute the total disagreements, error!\n", __LINE__, __func__);
		return FAILED;
	}

	// get discordant region count
	if(getDiscordantRegNum(&discordantRegNum, ratioRegionArray, ratioRegionNum, headSkipRegNum, tailSkipRegNum)==FAILED)
	{
		printf("line=%d, In %s(), cannot compute the discordant region count, error!\n", __LINE__, __func__);
		return FAILED;
	}

	insertSize = readSet->insertSize;
	standDev = readSet->standDev;

	startQueryPosLeft = queryIndel->leftMargin - insertSize;
	endQueryPosRight = queryIndel->rightMargin + insertSize;
	endQueryPosLeft = (startQueryPosLeft + endQueryPosRight) / 2;
	startQueryPosRight = endQueryPosLeft + 1;
	if(startQueryPosLeft<1)
		startQueryPosLeft = 1;
	if(endQueryPosRight>queryItem->queryLen)
		endQueryPosRight = queryItem->queryLen;


	// compute the fragment size of paired-end reads between the two regions
	if(computeFragSizeBothRegQueryIndel(queryIndel, startQueryPosLeft, endQueryPosLeft, startQueryPosRight, endQueryPosRight, queryItem, readSet)==FAILED)
	{
		printf("line=%d, In %s(), cannot compute the fragment size, error!\n", __LINE__, __func__);
		return FAILED;
	}

	if(queryIndel->pairNumLeft>0 || queryIndel->pairNumRight>0)
	{
		queryIndel->difFragSizeLeft = queryIndel->averFragSizeLeft - insertSize;
		queryIndel->difFragSizeRight = queryIndel->averFragSizeRight - insertSize;
		difFragSize = (queryIndel->difFragSizeLeft + queryIndel->difFragSizeRight) / 2;
		if(difFragSize<0)
			difFragSize = -difFragSize;
	}else
	{
		difFragSize = 0;
	}

	if((totalZeroCovNum>0 || discordantRegNum>0) && (difFragSize>2*standDev || difFragSize>0.2*insertSize))
		queryIndel->misassFlag = UNCERTAIN_MISASS;
	else if(totalDisagreeNum>0)
	{
		if(totalDisagreeNum>=2)
			queryIndel->misassFlag = UNCERTAIN_MISASS;
		else if(totalDisagreeNum==1 && queryIndel->disagreeRegSize<300 && (totalZeroCovNum>0 || discordantRegNum>0))
			queryIndel->misassFlag = UNCERTAIN_MISASS;
		else
			queryIndel->misassFlag = STRUCTURE_VARIATION;
	}else
		queryIndel->misassFlag = STRUCTURE_VARIATION;

	//printf("indel reg[%d, %d]: misassFlag=%d, totalDisagreeNum=%d, totalZeroCovNum=%d, discordantRegNum=%d, difFragSize=%.4f\n", queryIndel->leftMargin, queryIndel->rightMargin, queryIndel->misassFlag, totalDisagreeNum, totalZeroCovNum, discordantRegNum, difFragSize);

	return SUCCESSFUL;
}

/**
 * Compute the total disagreements for query indel.
 *  @return:
 *  	If succeeds, return SUCCESSFUL; otherwise, return FAILED.
 */
short computeTotalDisagreeNum(int32_t *totalDisagreeNum, int32_t *totalZeroCovNum, ratioRegion_t *ratioRegionArray, int32_t ratioRegionNum, int32_t headSkipRegNum, int32_t tailSkipRegNum)
{
	int32_t i, startRow, endRow, tmp;

	*totalDisagreeNum = 0;
	*totalZeroCovNum = 0;

	startRow = headSkipRegNum;
	endRow = ratioRegionNum - tailSkipRegNum - 1;
	if(startRow>=ratioRegionNum)
		startRow = ratioRegionNum - 1;
	if(endRow<0)
		endRow = 0;
	if(startRow>endRow)
	{
		tmp = startRow;
		startRow = endRow;
		endRow = tmp;
	}

	if(startRow<0 || endRow>=ratioRegionNum)
	{
		printf("line=%d, In %s(), startRow=%d, endRow=%d, ratioRegionNum=%d, error!\n", __LINE__, __func__, startRow, endRow, ratioRegionNum);
		return FAILED;
	}

	for(i=startRow; i<=endRow; i++)
	{
		*totalDisagreeNum += ratioRegionArray[i].disagreeNum;
		*totalZeroCovNum += ratioRegionArray[i].zeroCovNum;
	}

	return SUCCESSFUL;
}

/**
 * Get the discordant sub-region count for query indel.
 *  @return:
 *  	If succeeds, return SUCCESSFUL; otherwise, return FAILED.
 */
short getDiscordantRegNum(int32_t *discordantRegNum, ratioRegion_t *ratioRegionArray, int32_t ratioRegionNum, int32_t headSkipRegNum, int32_t tailSkipRegNum)
{
	int32_t i, startRow, endRow, tmp;
	ratioRegion_t *subReg;

	*discordantRegNum = 0;

	startRow = headSkipRegNum;
	endRow = ratioRegionNum - tailSkipRegNum - 1;
	if(startRow>=ratioRegionNum)
		startRow = ratioRegionNum - 1;
	if(endRow<0)
		endRow = 0;
	if(startRow>endRow)
	{
		tmp = startRow;
		startRow = endRow;
		endRow = tmp;
	}

	if(startRow<0 || endRow>=ratioRegionNum)
	{
		printf("line=%d, In %s(), startRow=%d, endRow=%d, ratioRegionNum=%d, error!\n", __LINE__, __func__, startRow, endRow, ratioRegionNum);
		return FAILED;
	}

	for(i=startRow; i<=endRow; i++)
	{
		subReg = ratioRegionArray + i;
		if(subReg->discorNum>=3 && subReg->discorRatio>0.1)
			(*discordantRegNum) ++;
	}

	return SUCCESSFUL;
}

/**
 * Get the multiple aligned sub-region count for query indel.
 *  @return:
 *  	If succeeds, return SUCCESSFUL; otherwise, return FAILED.
 */
short getMultiReadsRegNum(int32_t *multiReadsRegNum, ratioRegion_t *ratioRegionArray, int32_t ratioRegionNum, int32_t headSkipRegNum, int32_t tailSkipRegNum)
{
	int32_t i, startRow, endRow, tmp;
	ratioRegion_t *subReg;

	*multiReadsRegNum = 0;

	startRow = headSkipRegNum;
	endRow = ratioRegionNum - tailSkipRegNum - 1;
	if(startRow>=ratioRegionNum)
		startRow = ratioRegionNum - 1;
	if(endRow<0)
		endRow = 0;
	if(startRow>endRow)
	{
		tmp = startRow;
		startRow = endRow;
		endRow = tmp;
	}

	if(startRow<0 || endRow>=ratioRegionNum)
	{
		printf("line=%d, In %s(), startRow=%d, endRow=%d, ratioRegionNum=%d, error!\n", __LINE__, __func__, startRow, endRow, ratioRegionNum);
		return FAILED;
	}

	for(i=startRow; i<=endRow; i++)
	{
		subReg = ratioRegionArray + i;
		if(subReg->multiReadsRatio>0.05)
			(*multiReadsRegNum) ++;
	}

	return SUCCESSFUL;
}
