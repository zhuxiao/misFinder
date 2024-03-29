/*
 * methods.h
 *
 *  Created on: Nov 21, 2011
 *      Author: xiao
 */

#ifndef METHODS_H_
#define METHODS_H_ 1

#include <iostream>
#include <string>
#include <vector>

#include "structure.h"

using namespace std;

// ================= mfMain.cpp ====================
short parseCommandParasAndExe(int argc, char **argv);
short showUsageInfo();


// ================= mf.cpp ====================
int computeGlobalMetrics(int32_t operationMode, char *outputPathName, char *configFilePara, int minQueryLenThreshold, double matchPercentThreshold, int32_t threadNumPara, int32_t indelSizeThresPara, int32_t singleCellFlagPara);
short initGlobalParas(int32_t operationMode, char *outputPathName, char *configFilePara, int minQueryLenThreshold, double matchPercentThreshold, int32_t threadNumPara, int32_t indelSizeThresPara, int32_t singleCellFlagPara);
short setGlobalPath(const char *outPathStr);
void resetGlobalParas();
short generateConfigFile(const string &configFile);
short parseConfigFile(char *inputQueryFileInit, char *subjectSegsFile, readFile_t **readFileList, char *configFile);
short addToReadFileList(readFile_t **readFileList, char *readFiles);
short getReadsFileFormat(int32_t *readsFileFormatType, char **readFilesInput, int32_t readFileNum);
short checkFilesInConfigfile(int32_t operationMode, char *inputQueryFileInit, char *subjectsFile, readFile_t *readFileList);
short outputConfigInfo(int32_t operationMode, char *inputQueryFileInit, char *subjectsFile, readFile_t *readFileList);
short copyQueryFile(char *inputQueryFile, char *inputQueryFileInit);

// ================= subjectMerge.cpp ====================
short mergeRefSegmentsFasta(char *mergedSegFile, char *subjectsFile);
short initMemRefSegmentsFasta(char ***segFileArray, int32_t *segFileNum, char *subjectsFile);
void freeMemRefSegmentsFasta(char **segFileArray, int32_t segFileNum);
short getSegmentFileNum(int32_t *segmentFileNum, const char *subjectsFileName);
short mergeRefSegsFasta(const char *mergedSegFile, char **segFileArr, int64_t segFileNum);
short getMaxFileSizeByte(int64_t *maxFileSizeByte, char **segFileArray, int64_t fileNums);
short filterTailNonBases(char *segBuf, int64_t *bufLen);
short fillSegBuf(char *segBuf, int64_t *bufLen, char *segmentFile);
short fillSubjectSeqs(queryMatchInfo_t *queryMatchInfoSet, char *mergedSegFile);

// ================= generateAlign.cpp ====================
short generateAlignResult(char *outputPathStr, char *queryMatchInfoFile, char *inputBlastnFile, char *inputQueryFile, char *mergedSegFile, int32_t threadNum);
short initAlignFiles(char *perfectQueryFile, char *matchedQueryFile, char *disjunctQueryFile, char *unmatchedQueryFile, char *linkedQueryMatchInfoFile, char *parseResultFile, char *queryMatchInfoFileTmp, char *queryMatchInfoFile, char *outputPathStr);
short generateBlastnResult(const char *outputPathStr, const char *inputBlastnFile, const char *inputQueryFile, const char *mergedSegFile, int32_t threadNum);
short initThreadParasBlastn(pthread_t **threadArray, threadPara_t **threadParaArray, int32_t threadNum, const char *outputPathStr, const char *mergedSegFile);
short freeThreadParasBlastn(pthread_t **threadArray, threadPara_t **threadParaArray, int32_t threadNum);
short divideQueryFiles(threadPara_t *threadParaArray, int32_t threadNum, const char *inputQueryFile);
short getQueryOrderFlag(int32_t *sizeOrderFlag, const char *inputQueryFile);
short fillQuerySizeArray(int32_t *sizeArray, int64_t queryNum, const char *inputQueryFile);
short determineQuerySizeOrder(int32_t *sizeOrderFlag, int32_t *sizeArray, int64_t queryNum);
short computeSizeSubQueries(threadPara_t *threadParaArray, int32_t threadNum, int32_t sizeOrderFlag, const char *inputQueryFile);
short getSumLengthQuery(int64_t *sumQueryLen, int64_t *queryNum, const char *inputQueryFile);
short computeQuerySubSum(threadPara_t *threadParaArray, int32_t threadNum, int64_t sumQueryLen, int32_t sizeOrderFlag);
short smoothSampleRatio(double *sampleRatioArray, int32_t arraySize);
short generateSubQueries(threadPara_t *threadParaArray, int32_t threadNum, const char *inputQueryFile);
short getMaxQueryLenFromFile(int64_t *maxQueryLen, const char *queryFileName);
short getSingleFastaItemFromFile(FILE *fpQuery, char *queryHeadTitle, char *querySeq, int64_t *queryLen);
short createThreadsBlastn(pthread_t *threadArray, threadPara_t *threadParaArray, int32_t threadNum);
short waitThreads(pthread_t *threadArray, threadPara_t *threadParaArray, int32_t threadNum);
void* generateBlastnResultThread(void *arg);
short generateBlastnResultNoThread(const char *inputBlastnFile, const char *inputQueryFile, const char *mergedSegFile);
short mergeBlastnResults(const char *inputBlastnFile, threadPara_t *threadParaArray, int32_t threadNum);

// ================= parse.cpp ====================
short parseBlastn(char *parseResultFile, const char *blastnResultFile);
short readLine(int *status, char *str, int *len, int maxChNum, FILE *fp);

// ================= classfy.cpp ====================
short classifyQueries(char *perfectQueryFile, char *matchedQueryFile, char *disjunctQueryFile, char *unmatchedQueryFile, char *queryMatchFile, char *queryMatchInfoFileUpdated, char *queryMatchInfoFile, char *inputQueryFile, char *mergedSegFile);
short initMemClassification(FILE **fpPerfectQuery, FILE **fpMatchedQuery, FILE **fpDisjunctQuery, FILE **fpUnmatchedQuery, FILE **fpQueryMatch, queryMatchInfo_t **queryMatchInfoSet, segLinkSet_t **segLinkSet, char *perfectQueryFile, char *matchedQueryFile, char *disjunctQueryFile, char *unmatchedQueryFile, char *queryMatchFile, char *parseResultFile, char *inputQueryFile, char *mergedSegFile);
void freeMemClassification(FILE **fpPerfectQuery, FILE **fpMatchedQuery, FILE **fpDisjunctQuery, FILE **fpUnmatchedQuery, FILE **fpQueryMatch, queryMatchInfo_t **queryMatchInfoSet, segLinkSet_t **segLinkSet);
short determineMatchKind(segLinkSet_t *segLinkSet, querySubject_t *pQuerySubject, int64_t queryLen, matchItem_t *matchItemArray, subject_t *subjectArray);
short getAdjacentSegment(int64_t *adjacentRowID, int64_t startRowID, matchItem_t *matchItemArray, int64_t matchItemNum, char *pUsedArray, int64_t queryLen, int64_t subjectLen, int subjectCircularFlag, int selectionRound);
//short determineSameOrder(int *orderSameFlag, segmentLink_t *segmentLinkArray, int headRowSegmentLinkArray);
short addNewItemSegLinkArray(segLinkSet_t *segLinkSet, int32_t dataRow, int32_t addOrder, int32_t headTailFlag);
short fillBrokenSegsLinkArray(segLinkSet_t *segLinkSet, matchItem_t *matchItemArray, int64_t matchItemNum, char *pUsedArray);
short selectValidBrokenSegRow(int32_t *brokenRowID, int32_t dataRow, int32_t nextDataRow, matchItem_t *matchItemArray, int64_t matchItemNum, char *pUsedArray);
short removeRedundantSegments(segLinkSet_t *segLinkSet, matchItem_t *matchItemArray, int64_t queryLen, int64_t subjectLen);
short determinePerfectSegmentFlag(int32_t *perfectFlag, segLinkSet_t *segLinkSet, matchItem_t *matchItemArray);
short determineTotalLenEqualToQueryFlag(int32_t *totalLenEqualToQueryFlag, segLinkSet_t *segLinkSet, int32_t queryLen, matchItem_t *matchItemArray);
short determineCircularQueryFlag(int32_t *circularFlag, segLinkSet_t *segLinkSet, matchItem_t *matchItemArray, int64_t subjectLen);
short computeAlignedSegLen(int64_t *totalAlignedSegLen, segLinkSet_t *segLinkSet, matchItem_t *pMatchItemArray);
short checkDisjunctFlag(int32_t *disjunctFlag, int32_t circularFlag, int64_t subjectLen, segLinkSet_t *segLinkSet, matchItem_t *pMatchItemArray);
short outputGlobalMatchResultToFile(FILE *fpPerfectQuery, FILE *fpMatchedQuery, FILE *fpDisjunctQuery, FILE *fpUnmatchedQuery, FILE *fpQueryMatch, queryMatchInfo_t *queryMatchInfoSet);
short saveValidSegments(querySubject_t *pQuerySubject, matchItem_t *matchItemArray, segLinkSet_t *segLinkSet);
short determineBestQuerySubjectItem(query_t *pQuery);
short removeRedundantAlignSegsSingleQuery(query_t *pQuery);
short adjustValidSegOrderSingleQuery(query_t *queryItem);

// ================= globalSeg.cpp ====================
short fillGlobalAlignSeg(queryMatchInfo_t *queryMatchInfoSet, segLinkSet_t *segLinkSet);
short fillGlobalSegSingleQuery(query_t *queryItem, subject_t *subjectArray, segLinkSet_t *segLinkSet);
short generateGlobalSegArrayFromValidSegArray(query_t *queryItem, int32_t bestSubjectID, validSegment_t *validSegArray, int32_t validSegNum);
short initWholeGlobalSegArray(globalValidSeg_t **wholeGlobalSegArray, int32_t *maxGlobalSegNum, query_t *queryItem);
short sortGlobalSegArray(globalValidSeg_t *wholeGlobalSegArray, int32_t arraySize);
short selectionSortGlobalSegArray(globalValidSeg_t *wholeGlobalSegArray, int32_t arraySize);
short radixSortGlobalSegArray(globalValidSeg_t *segArray, int32_t itemNum);
short getGlobalSegSingleQuery(globalValidSeg_t *globalSegArrayBuf, int32_t *globalSegNum, globalValidSeg_t *wholeGlobalSegArray, int32_t maxGlobalSegNum, segLinkSet_t *segLinkSet, query_t *queryItem, subject_t *subjectArray);
short fillLinkArrayGivenReg(segLinkSet_t *segLinkSet, int32_t startQPos, int32_t endQPos, globalValidSeg_t *globalSegArray, int8_t *usedFlagArray, int32_t segArraySize, subject_t *subjectArray);
int32_t isValidGapSeg(globalValidSeg_t *globalSegNode, int32_t startQPos, int32_t endQPos);
short copyNewItemsFromLinkArray(globalValidSeg_t *globalSegArrayBuf, int32_t *globalSegNum, segLinkSet_t *segLinkSet, globalValidSeg_t *wholeGlobalSegArray);
short addValidSegItemToGlobalArray(globalValidSeg_t *globalValidSegArray, int32_t *globalSegNum, validSegment_t *validSegNode, int32_t subjectID);
short getAdjacentRowGlobalSeg(int32_t *adjacentRow, int32_t startSegRow, int32_t startQPos, int32_t endQPos, globalValidSeg_t *globalSegArray, int8_t *usedFlagArray, int32_t segArraySize, subject_t *subjectArray, int32_t linkRound);
short removeRedundantGlobalSegSingleQuery(globalValidSeg_t *globalSegArray, int32_t *globalSegNum, query_t *queryItem);
short adjustGlobalSegOrderSingleQuery(globalValidSeg_t *globalSegArray, int32_t globalSegNum);
short generateGlobalSegArraySingleQuery(query_t *queryItem, globalValidSeg_t *globalSegArrayBuf, int32_t globalSegNum);
short trimAlignInfoQueryEnds(query_t *queryItem, subject_t *subjectArray);
short trimAlignLeftSegEnd(globalValidSeg_t *globalSeg, char *querySeq, char *subjectSeq);
short trimAlignRightSegEnd(globalValidSeg_t *globalSeg, char *querySeq, char *subjectSeq);
short determineGlobalMatchKindSingleQuery(query_t *queryItem);
short getBaseCovPercentSingleQuery(double *baseCovSingleQuery, query_t *queryItem);

// ================= convert.cpp ====================
short convertMatchInfo(char *queryMatchInfoFile, char *parseResultFile);
short initMemConvertMatchInfo(queryMatchInfo_t **queryMatchInfoSet, tmpQuery_t **tmpQueryArray, int64_t *itemNumTmpQueryArray, char *parseResultFile);
void freeMemConvertMatchInfo(queryMatchInfo_t **queryMatchInfoSet, tmpQuery_t **tmpQueryArray, int64_t *itemNumTmpQueryArray);
short fillDataTmpQueryArray(tmpQuery_t *tmpQueryArray, int64_t itemNumTmpQueryArray, queryMatchInfo_t *queryMatchInfoSet, char *parseResultFile);
short fillSubjectArray(subject_t *subjectArray, int64_t itemNumSubjectArray, char *parseResultFile);
short fillSingleTmpQueryMatchInfo(tmpQuery_t *pTmpQuery, matchItem_t *pMatchItemArray, int64_t itemNumMatchItemArray, int64_t tmpQueryID, subject_t *subjectArray, int64_t itemNumSubjectArray, FILE *fpParsedResult);
short convertQueryMatchInfo(queryMatchInfo_t *queryMatchInfoSet, tmpQuery_t *tmpQueryArray, int64_t itemNumTmpQueryArray);
short getItemNums(int64_t *itemNumSubjectArray, int64_t *itemNumQueryArray, int64_t *itemNumTmpQueryArray, int64_t *itemNumMatchItemArray, char *parseResultFile);
short getSubjectNum(int64_t *itemNumSubjectArray, char *parseResultFile);
short getFirstQueryTitle(char *queryTitle, char *parseResultFile);
short getQuerySubjectNumAndMatchItemNum(int64_t *itemNumQueryArray, int64_t *tmpQuerySubjectNum, int64_t *itemNumMatchItemArray, char *parseResultFile);
void releaseQueryMatchInfo(queryMatchInfo_t **queryMatchInfoSet);
short saveQueryMatchInfoToFile(char *queryMatchInfoFile, queryMatchInfo_t *queryMatchInfoSet);
short loadQueryMatchInfoFromFile(queryMatchInfo_t **queryMatchInfoSet, char *queryMatchInfoFile);

// ================= lenStatistics.cpp ====================
short queryLenStatistics(metrics_t *queryMetrics, queryMatchInfo_t *queryMatchInfoSet, char *sortedQueryFile);
short initMemLenStatistics(queryLenStatistic_t **lenStatisticArray, queryLenStatistic_t **lenStatisitcBufArray, int64_t *itemNumLenStatisticArray, int64_t itemNum);
void freeMemLenStatistics(queryLenStatistic_t **lenStatisticArray, queryLenStatistic_t **lenStatisitcBufArray);
short fillQueryDataLenStatistic(queryLenStatistic_t *lenStatisticArray, int64_t itemNumLenStatisticArray, query_t *queryArray, int64_t itemNumQueryArray);
short radixSortOfLengths(queryLenStatistic_t *lenStatisticArray, queryLenStatistic_t *lenStatisitcBufArray, int64_t itemNumLenStatisticArray);
short saveSortedQueries(char *sortedQueriesFile, queryLenStatistic_t *lenStatisticArray, int64_t itemNumLenStatisticArray, query_t *queryArray);
short computeLenStatisticsQueryMetrics(metrics_t *queryMetrics, queryLenStatistic_t *lenStatisticArray, int64_t itemNumLenStatisticArray);
short computeReferenceCovRatio(metrics_t *queryMetrics, char *refDeletionFile, subject_t *subjectArray, int64_t itemNumSubjectArray, query_t *queryArray, int64_t itemNumQueryArray, char *blastnResultFile);
short fillReferenceBaseFlag(metrics_t *queryMetrics,  char *refDeletionFile, subject_t *subjectArray, int64_t itemNumSubjectArray, query_t *queryArray, int64_t itemNumQueryArray, char *blastnResultFile);
//short checkValidQuerySubject(int *validQuerySubjectFlag, int64_t querySubjectNum, query_t *queryArray, int64_t itemNumQueryArray, subject_t *subjectArray, int64_t itemNumSubjectArray);
//short checkValidSegment(int *validSegmentFlag, int64_t querySubjectNum, int strand, int32_t matchLen, int32_t totalMatchLen, int32_t gapNum, int32_t startQueryPos, int32_t startSubjectPos, query_t *queryArray, int64_t itemNumQueryArray, subject_t *subjectArray, int64_t itemNumSubjectArray);
short updateGenomeBaseFlag(metrics_t *queryMetrics, FILE *fpRefDel, char **alignBaseBlock, int alignBlockLen, int querySubjectNum, int strand, int startSubjectPos, int endSubjectPos, int startQueryPos, int endQueryPos, subject_t *subjectArray, int64_t itemNumSubjectArray, query_t *queryArray, int64_t itemNumQueryArray);
short computeRefCovRatioByBaseFlag(metrics_t *queryMetrics);
short computeLenStatisticsFromFastaFile(char *fastaFileName, const string &comments);
short fillDataLenStatistic(queryLenStatistic_t *lenStatisticArray, int64_t itemNumLenStatisticArray, char *fastaFileName);
short computeLenStatistics(queryLenStatistic_t *lenStatisticArray, int64_t itemNumLenStatisticArray, const string &comments);

// ================= accuracyStatistics.cpp ====================
short getQueryMetrics(char *outputPathStr, char *queryMatchInfoFile, char *blastnResultFile, char *mergedSegmentsFile);
short initMemQueryMetrics(metrics_t **queryMetrics, queryMatchInfo_t **queryMatchInfoSet, char *queryStatisticFile, char *sortedQueryFile, char *refDeletionFile, char *queryMatchInfoFile, char *outputPathStr);
short allocateQueryMetrics(metrics_t **queryMetrics, subject_t *subjectArray, int64_t itemNumSubjectArray);
void releaseQueryMetrics(metrics_t **queryMetrics);
void freeMemQueryMetrics(metrics_t **queryMetrics, queryMatchInfo_t **queryMatchInfoSet);
short saveQueryStatisticsToFile(char *queryStatisticFile, metrics_t *queryMetrics);
short queryAccuracyStatistics(metrics_t *queryMetrics, query_t *queryArray, int64_t itemNumQueryArray);
short computeGCRatio(metrics_t *queryMetrics, char *mergedSegmentsFile);

// ================= reads.cpp ====================
short constructReadsetArray(readSetArr_t **readSetArray, readFile_t *readFileList);
short constructReadset(readSet_t *readSet, readFile_t *readFileNode);
short constructReadsetByPEFastaSeparate(readSet_t *readSet, char **readsFileNames, int32_t readsFileNum);
short constructReadsetByPEFastaInterleaved(readSet_t *readSet, char **readsFileNames, int32_t readsFileNum);
short constructReadsetByPEFastqSeparate(readSet_t *readSet, char **readsFileNames, int32_t readsFileNum);
short constructReadsetByPEFastqInterleaved(readSet_t *readSet, char **readsFileNames, int32_t readsFileNum);
short fillReadsToBufFasta(FILE *fpReads, readBuf_t *pBuf, int32_t *readsNum);
short fillReadsToBufFastq(FILE *fpReads, readBuf_t *pBuf, int32_t *readsNum);
short getSingleReadFasta(FILE *fpPE, readBuf_t *pReadBuf);
short getSingleReadFastq(FILE *fpPE, readBuf_t *pReadBuf);
short generateReadseqIntInBuf(readBuf_t *readBuf, int32_t tmpReadsNum, readSet_t *readSet);
short computeValidFlagBufRead(readBuf_t *pBufRead);
int32_t getUnknownBaseNum(char *seq);
float getRatioBase(char *seq, char targetBase);
short addReadToReadset(readBuf_t *pBufRead, readSet_t *readSet);
short initReadSet(readSet_t *pReadSet);
short initReadBlockInReadset(readSet_t *pReadSet);
short addNewBlockRead(readSet_t *pReadSet);
short initReadseqBlockInReadset(readSet_t *pReadSet);
short initReadMatchInfoBlockInReadset(readSet_t *pReadSet);
void releaseReadMatchInfoBlockInReadset(readSet_t *pReadSet);
short addNewBlockReadseq(readSet_t *pReadSet);
short initReadseqHashtableInReadset(readSet_t *pReadSet);
short initReadseqHashItemBlockInGraph(readSet_t *pReadSet);
short addNewBlockReadseqHashItem(readSet_t *pReadSet);
void releaseReadsetArray(readSetArr_t **pReadSetArray);
void releaseReadset(readSet_t *pReadSet);
void releaseHashItemReadset(readSet_t *readSet);
uint64_t readseqHashInt(uint64_t *seqInt, int32_t baseNum, int32_t entriesNum);
short generateReadseqInt(uint64_t *seqInt, char *seq, int32_t seqLen, int32_t entriesNum);
readseqHashItem_t *getReadseqHashItemByHash(uint64_t hashvalue, uint64_t *readseqInt, int32_t seqLen, int32_t entriesNum, readSet_t *readSet);
short identicalReadseq(uint64_t *kmerSeqInt1, uint64_t *kmerSeqInt2, int32_t entriesNum);
short getReadBaseByInt(char *baseSeq, uint64_t *readseqInt, int32_t seqLen);
short getReadBaseFromPosByInt(char *readBaseSeq, uint64_t *readseqInt, int32_t seqLen, int32_t startBasePos, int32_t baseNum);
short getReverseReadBaseByInt(char *baseSeq, uint64_t *readseqInt, int32_t seqLen);
short getReverseReadBaseFromPosByInt(char *readBaseSeq, uint64_t *readseqInt, int32_t seqLen, int32_t startRevBasePos, int32_t baseNum);
short getReverseReadseqInt(uint64_t *readseqIntRev, uint64_t *readseqInt, int32_t seqLen);
short replaceUnknownBasesInReads(char *seq, int32_t nBaseNum);
short computeMaxReadLenInReadset(readSet_t *readSet);
short getReadByReadID(read_t **read, int64_t readID, readSet_t *readSet);

// ================= misass.cpp ====================
short validateMisassQueries(char *outputPathStr, char *newQueryFile, char *queryMatchInfoFile, char *inputQueryFile, char *mergedSegFile, readFile_t *readFileList);
short initMemMisass(queryMatchInfo_t **queryMatchInfoSet, char *errorsFile, char *svFile, char *misUncertainFile, char *gapFile, char *inputQueryFile, char *mergedSegFile, char *queryMatchInfoFile, char *outputPathStr);
void freeMemMisass(queryMatchInfo_t **queryMatchInfoSet, readSetArr_t **readSetArray, queryIndex_t **queryIndex);
short computePotentMisassNum(queryMatchInfo_t *queryMatchInfoSet);
short computeMisassQueries(queryMatchInfo_t *queryMatchInfoSet, readSetArr_t *readSetArray, int32_t threadNum);
short initThreadParasMisass(pthread_t **threadArray, threadPara_t **threadParaArray, int32_t threadNum, queryMatchInfo_t *queryMatchInfoSet, readSet_t *readSet);
short freeThreadParasMisass(pthread_t **threadArray, threadPara_t **threadParaArray, int32_t threadNum);
short createThreadsMisass(pthread_t *threadArray, threadPara_t *threadParaArray, int32_t threadNum);
void* computeMisassQueriesSingleThread(void *arg);
short computeSingleMisassQuery(query_t *queryItem, subject_t *subjectArray, readSet_t *readSet);
short computeBaseCovSingleQuery(baseCov_t *baseCovArray, query_t *queryItem, readSet_t *readSet);
short outputBaseCovSingleQueryToFile(char *covFileName, baseCov_t *baseCovArray, int32_t arraySize);
short outputCovReadsToFile(char *covReadsFileName, int32_t basePos, char base, query_t *queryItem, readSet_t *readSet);
short computeRegCovSingleQuery(query_t *queryItem, baseCov_t *baseCovArray, int32_t arraySize);
short outputRegCovSingleQueryToFile(char *regCovFileName, baseCov_t *baseCovArray, int32_t arraySize);
short computeDisagreements(int32_t *disagreeNum, int32_t *zeroCovNum, int32_t *disagreeRegSize, baseCov_t *baseCovArray, int32_t startRow, int32_t endRow, int32_t printFlag);
short computeAbnormalCovRegNum(int32_t *highCovRegNum, int32_t *lowCovRegNum, baseCov_t *baseCovArray, int32_t startRow, int32_t endRow, query_t *queryItem, int32_t skipEndFlag, int32_t printFlag);
short computeMisassFlagAlignSeg(int32_t *misassFlag, globalValidSeg_t *leftAlignSeg, globalValidSeg_t *rightAlignSeg, int32_t subjectLen, int32_t circularFlag);
short getGapFlagMisInfo(int32_t *gapFlag, globalValidSeg_t *leftAlignSeg, globalValidSeg_t *rightAlignSeg, query_t *queryItem);
short computeAlignSegMargin(int64_t *leftMargin, int64_t *rightMargin, globalValidSeg_t *leftAlignSeg, globalValidSeg_t *rightAlignSeg, int32_t difQuery, int32_t difSubject, query_t *queryItem, subject_t *subjectArray);
short getRightAlignSeqs(char **queryAlignSeq, char **subjectAlignSeq, int32_t *queryAlignSeqLen, int32_t *subjectAlignSeqLen, int64_t *leftQueryPos, int64_t *rightQueryPos, int64_t *leftSubjectPos, int64_t *rightSubjectPos, globalValidSeg_t *leftAlignSeg, globalValidSeg_t *rightAlignSeg, query_t *queryItem, subject_t *subjectArray);
short getLeftAlignSeqs(char **queryAlignSeq, char **subjectAlignSeq, int32_t *queryAlignSeqLen, int32_t *subjectAlignSeqLen, int64_t *leftQueryPos, int64_t *rightQueryPos, int64_t *leftSubjectPos, int64_t *rightSubjectPos, globalValidSeg_t *leftAlignSeg, globalValidSeg_t *rightAlignSeg, query_t *queryItem, subject_t *subjectArray);
short reverseSeq(char *seq, int32_t seqLen);
short computeSeqAlignment(char **pAlignResultArray, int32_t *overlapLen, int32_t *mismatchNum, int32_t *leftShiftLen1, int32_t *leftShiftLen2, int32_t *rightShiftLen1, int32_t *rightShiftLen2, char *seq1, char *seq2, int32_t seqLen1, int32_t seqLen2, int32_t printFlag);
short computeRightMargin(int64_t *rightMargin, int64_t *rightMarginSubject, char **alignResultArray, int32_t overlapLen, int64_t leftQueryPos, int64_t rightQueryPos, int64_t leftSubjectPos, int64_t rightSubjectPos, int32_t queryLeftShiftLen, int32_t subjectLeftShiftLen, globalValidSeg_t *globalSeg);
short computeLeftMargin(int64_t *leftMargin, int64_t *leftMarginSubject, char **alignResultArray, int32_t overlapLen, int64_t leftQueryPos, int64_t rightQueryPos, int64_t leftSubjectPos, int64_t rightSubjectPos, int32_t queryRightShiftLen, int32_t subjectRightShiftLen, globalValidSeg_t *globalSeg);
short computeBreakpointRatios(query_t *queryItem, int32_t misjoinNum, readSet_t *readSet);
short initRatioRegionArrayBreakpoint(ratioRegion_t **ratioRegionArray, int32_t ratioRegionNum, query_t *queryItem);
short fillRatioRegionArray(ratioRegion_t *ratioRegionArray, int32_t ratioRegionNum, query_t *queryItem, readSet_t *readSet, int32_t computeOtherMateFlag);
short computeRatios(ratioRegion_t *ratioRegionArray, int32_t ratioRegionNum);
short updateRatiosInQuery(query_t *queryItem, ratioRegion_t *ratioRegionArray, int32_t ratioRegionNum);
void outputRatioRegionArray(ratioRegion_t *ratioRegionArray, int32_t ratioRegionNum);
short determineMisassFlag(query_t *queryItem, int32_t misjoinRegNum, baseCov_t *baseCovArray, readSet_t *readSet);
short saveMisassQueries(char *errorsFile, char *svFile, char *misUncertainFile, char *gapFile, queryMatchInfo_t *queryMatchInfoSet);
short saveNewQueries(char *newQueryFile, queryMatchInfo_t *queryMatchInfoSet);

// ================= queryIndex.cpp ====================
short buildQueryIndex(queryIndex_t **queryIndex, queryMatchInfo_t *queryMatchInfoSet);
short initQueryIndex(queryIndex_t **queryIndex);
void releaseQueryIndex(queryIndex_t **queryIndex);
short initQueryKmerBlock(queryIndex_t *queryIndex);
short addNewBlockQueryKmer(queryIndex_t *queryIndex);
short initQueryKmerseqBlock(queryIndex_t *queryIndex);
short addNewBlockQueryKmerSeq(queryIndex_t *queryIndex);
short countQueryKmerOccs(queryIndex_t *queryIndex, queryMatchInfo_t *queryMatchInfoSet);
short getStartEndQueryPos(int32_t *startQueryPos, int32_t *endQueryPos, char *querySeq, int32_t queryLen, int32_t kmerSize);
short checkValidQueryBase(int32_t *validFlag, char base);
short getNextValidQueryBasePos(int32_t *newBasePos, char *querySeq, int32_t basePos, int32_t endBasePos, int32_t kmerSize);
uint64_t kmerhashInt(uint64_t *seqInt, int32_t entriesPerKmer, int32_t lastEntryBaseNum, int64_t hashArraySize);
short countQueryKmer(uint64_t hashcode, uint64_t *kmerSeqInt, queryIndex_t *queryIndex);
queryKmer_t *getQueryKmerByHash(uint64_t hashvalue, uint64_t *kmerSeqInt, const queryIndex_t *queryIndex);
short identicalKmerSeq(uint64_t *kmerSeqInt1, uint64_t *kmerSeqInt2, int32_t entriesNum);
short addQueryKmerQuerypos(queryIndex_t *queryIndex, queryMatchInfo_t *queryMatchInfoSet);
short initQueryposBlocksInQueryIndex(queryIndex_t *queryIndex);
short addNewBlockQuerypos(queryIndex_t *queryIndex);
short addQueryKmer(uint64_t hashcode, uint64_t *kmerSeqInt, int32_t queryID, int32_t queryPos, queryIndex_t *queryIndex);
short getKmerBaseByInt(char *baseSeq, uint64_t *kmerSeqInt, int32_t entriesPerKmer, int32_t lastEntryBaseNum);
short fillQueries(queryMatchInfo_t *queryMatchInfoSet, char *inputQueryFile);

// ================= map.cpp ====================
short mapReads(queryMatchInfo_t *queryMatchInfoSet, readSetArr_t *readSetArray, queryIndex_t *queryIndex, int32_t threadNum);
short mapReadsSingleReadset(queryMatchInfo_t *queryMatchInfoSet, readSet_t *readSet, queryIndex_t *queryIndex, int32_t threadNum);
short initCovFlagArray(queryMatchInfo_t *queryMatchInfoSet);
void freeCovFlagArray(queryMatchInfo_t *queryMatchInfoSet);
short initThreadParasMap(pthread_t **threadArray, threadPara_t **threadParaArray, int32_t threadNum, queryMatchInfo_t *queryMatchInfoSet, readSet_t *readSet, queryIndex_t *queryIndex);
short freeThreadParasMap(pthread_t **threadArray, threadPara_t **threadParaArray, int32_t threadNum);
short mapReadsOp(pthread_t *threadArray, threadPara_t *threadParaArray, int32_t threadNum, int32_t uniqueMapOpFlag);
short initMutexMemMap(threadPara_t *threadParaArray, int32_t threadNum);
short freeMutexMemMap(threadPara_t *threadParaArray, int32_t threadNum);
short computeTotalAlignedReadsNum(readSet_t *readSet);
short createThreadsMap(pthread_t *threadArray, threadPara_t *threadParaArray, int32_t threadNum, int32_t uniqueMapOpFlag);
void* mapReadsOpSingleThread(void *arg);
short getMaxArraySizeFromQueryIndex(int32_t *maxArraySize, queryIndex_t *queryIndex);
short estimateInsertSize(queryMatchInfo_t *queryMatchInfoSet, readSetArr_t *readSetArray, queryIndex_t *queryIndex);
short estimateInsertSizeSingleReadset(queryMatchInfo_t *queryMatchInfoSet, readSet_t *readSet, queryIndex_t *queryIndex);
short mapSingleReadToQueries(uint64_t rid, read_t *pRead, readSet_t *readSet, alignMatchItem_t *matchResultArray, alignMatchItem_t *matchResultArrayBuf1, alignMatchItem_t *matchResultArrayBuf2, int32_t *matchItemNum, queryIndex_t *queryIndex, query_t *queryArray, int32_t mismatchNumThreshold);
short mapSingleReadToQueriesPerfect(int64_t rid, read_t *pRead, readSet_t *readSet, alignMatchItem_t *matchResultArray, alignMatchItem_t *matchResultArrayBuf1, alignMatchItem_t *matchResultArrayBuf2, int32_t *matchItemNum, queryIndex_t *queryIndex);
short getMatchedQueryPosPerfect(alignMatchItem_t *matchResultArray, alignMatchItem_t *matchResultArrayBuf1, alignMatchItem_t *matchResultArrayBuf2, int32_t *matchItemNum, uint64_t *readSeqInt, int32_t seqLen, queryIndex_t *queryIndex);
short mapSingleReadToQueriesWithMismatch(int64_t rid, read_t *pRead, readSet_t *readSet, alignMatchItem_t *matchResultArray, alignMatchItem_t *matchResultArrayBuf, int32_t *matchItemNum, queryIndex_t *queryIndex, query_t *queryArray, int32_t mismatchNumThreshold);
short getMatchedQueryPosWithMismatch(alignMatchItem_t *matchResultArray, int32_t *matchItemNum, uint64_t *readSeqInt, int32_t seqLen, int32_t orientation, const queryIndex_t *queryIndex, query_t *queryArray, int32_t mismatchNumThreshold);
short isProcessedMatchItem(int32_t *processedFlag, int32_t startAlignQueryPos, int32_t startBasePos, alignMatchItem_t *matchResultArray, int32_t matchItemNum);
short getMatedNumAlignResult(int32_t *matedNum, alignMatchItem_t *matchResultArray1, alignMatchItem_t *matchResultArray2, int32_t matchItemNum1, int32_t matchItemNum2, int32_t seqLen1, int32_t seqLen2, double insertSize, double standDev);
short computeUniqueMapFlags(int32_t *uniqueMapFlagArray, int32_t matedNum, alignMatchItem_t *matchResultArray1, alignMatchItem_t *matchResultArray2, int32_t matchItemNum1, int32_t matchItemNum2);
short selectMatedMatchPosPE(alignMatchItem_t *matchResultArray1, alignMatchItem_t *matchResultArray2, int32_t *matchItemNum1, int32_t *matchItemNum2, int32_t matedNum, int32_t seqLen1, int32_t seqLen2, double insertSize, double standDev, int32_t uniqueMapOpFlag, queryMatchInfo_t *queryMatchInfoSet, threadPara_t *threadPara);
short getZeroCovBaseNumSingleRead(int32_t *zeroCovBaseNum, alignMatchItem_t *alignMatchItem, queryMatchInfo_t *queryMatchInfoSet, threadPara_t *threadPara);
short selectBestAlignInfoSingleRead(alignMatchItem_t *matchResultArray, int32_t *matchItemNum, int32_t uniqueMapOpFlag, queryMatchInfo_t *queryMatchInfoSet, threadPara_t *threadPara);
short getMoreAlignItemsFromUnmatedPE(int32_t *matedNum, int64_t readID1, int64_t readID2, alignMatchItem_t *matchResultArray1, alignMatchItem_t *matchResultArray2, int32_t *matchItemNum1, int32_t *matchItemNum2, int32_t mismatchNumThreshold, queryMatchInfo_t *queryMatchInfoSet, readSet_t *readSet);
short getAlignItemsFromUnmatedEnd(int32_t *matedNum, read_t *pRead, alignMatchItem_t *matchResultArray, int32_t *matchItemNum, read_t *pReadBased, alignMatchItem_t *matchResultArrayBased, int32_t matchItemNumBased, int32_t mismatchNumThreshold, queryMatchInfo_t *queryMatchInfoSet, readSet_t *readSet);
short getAlignItemsFromSingleUnmatedPos(int32_t *matedNum, read_t *pRead, alignMatchItem_t *matchResultArray, int32_t *matchItemNum, read_t *pReadBased, alignMatchItem_t *matchResultArrayBased, int32_t itemRowBased, int32_t mismatchNumThreshold, queryMatchInfo_t *queryMatchInfoSet, readSet_t *readSet);
short updateQueryCovFlag(queryMatchInfo_t *queryMatchInfoSet, alignMatchItem_t *alignMatchItem, threadPara_t *threadPara);
short generateKmerSeqIntFromReadset(uint64_t *seqInt, uint64_t *readseq, int32_t startReadPos, int32_t kmerSize, int32_t entriesNumRead, int32_t baseNumLastEntry);
short getRowRangeMatchArray(int32_t *startRow, int32_t *endRow, alignMatchItem_t *matchResultArray, int32_t arraySize, int32_t queryID);
short fillReadMatchInfoQueries(queryMatchInfo_t *queryMatchInfoSet, readSetArr_t *readSetArray);
short initQueryReadSets(queryMatchInfo_t *queryMatchInfoSet, readSetArr_t *readSetArray);
short fillQueryReadSet(queryMatchInfo_t *queryMatchInfoSet, readSet_t *readSet);
short sortQueryReadSets(queryMatchInfo_t *queryMatchInfoSet);
short radixSortQueryReadArray(queryRead_t *queryReadArray, queryRead_t *queryReadArrayBuf, int32_t itemNum);
short sortMatchResults(alignMatchItem_t *matchResultArray, alignMatchItem_t *matchResultArrayBuf, int32_t matchItemNum);
short selectionSortMatchResults(alignMatchItem_t *matchResultArray, int32_t matchItemNum);
short radixSortMatchResults(alignMatchItem_t *matchResultArray, alignMatchItem_t *matchResultArrayBuf, int32_t itemNum);
short outputQueryReadArray(char *outfile, queryMatchInfo_t *queryMatchInfoSet, readSet_t *readSet);
short getUniqueMapFlag(int32_t *uniqueMapFlag, int64_t readID, readSet_t *readSet);
short getPairedMatchInfo(readMatchInfo_t **pReadMatchInfoPaired, int64_t readID, readSet_t *readSet);

// ================= indel.cpp ====================
short determineQueryIndel(query_t *queryItem, baseCov_t *baseCovArray, subject_t *subjectArray, readSet_t *readSet);
short confirmQueryIndelKind(queryIndel_t *queryIndel, query_t *queryItem, int32_t innerFlag, baseCov_t *baseCovArray, subject_t *subjectArray, readSet_t *readSet);
short computeFragSizeLeftRegQueryIndel(queryIndel_t *queryIndel, int32_t startQueryPosLeft, int32_t endQueryPosLeft, query_t *queryItem, readSet_t *readSet);
short computeFragSizeRightRegQueryIndel(queryIndel_t *queryIndel, int32_t startQueryPosRight, int32_t endQueryPosRight, query_t *queryItem, readSet_t *readSet);
short computeFragSizeBothRegQueryIndel(queryIndel_t *queryIndel, int32_t startQueryPosLeft, int32_t endQueryPosLeft, int32_t startQueryPosRight, int32_t endQueryPosRight, query_t *queryItem, readSet_t *readSet);
short determineFinalQueryIndelKind(queryIndel_t *queryIndel, int32_t innerFlag, double insertSize, double standDev);
short computeStatisticsRatioRegionQueryIndel(queryIndel_t *queryIndel, baseCov_t *baseCovArray, readSet_t *readSet, query_t *queryItem);
short statisticsSummaryInRatioRegQueryIndel(queryIndel_t *queryIndel, ratioRegion_t *ratioRegionArray, int32_t ratioRegionNum, int32_t subRegSize, query_t *queryItem);
short determineFinalQueryIndelKindUnmatched(queryIndel_t *queryIndel);

// ================= sv.cpp ====================
short computeSVInQueries(queryMatchInfo_t *queryMatchInfoSet, readSetArr_t *readSetArray, int32_t threadNum);
short initThreadParasSV(pthread_t **threadArray, threadPara_t **threadParaArray, int32_t threadNum, queryMatchInfo_t *queryMatchInfoSet, readSet_t *readSet);
short createThreadsSV(pthread_t *threadArray, threadPara_t *threadParaArray, int32_t threadNum);
void* computeSVInQueriesSingleThread(void *arg);
short computeSVInSingleQuery(query_t *queryItem, subject_t *subjectArray, readSet_t *readSet);
short determineSVInSingleQuery(query_t *queryItem, subject_t *subjectArray, baseCov_t *baseCovArray, readSet_t *readSet);
short determineSVMisjoinReg(query_t *queryItem, subject_t *subjectArray, baseCov_t *baseCovArray, readSet_t *readSet);
short determineSVIndelReg(query_t *queryItem, subject_t *subjectArray, baseCov_t *baseCovArray, readSet_t *readSet);
short checkSVRegMisjoin(queryMargin_t *queryMargin, baseCov_t *baseCovArray, query_t *queryItem, readSet_t *readSet);
short checkSVRegQueryIndel(queryIndel_t *queryIndel, baseCov_t *baseCovArray, query_t *queryItem, readSet_t *readSet);
short initRatioRegQueryIndel(ratioRegion_t **ratioRegionArray, int32_t *ratioRegionNum, int32_t startQueryPos, int32_t endQueryPos, int32_t subRegSize);
short computeDisagreeNumRatioRegs(ratioRegion_t *ratioRegionArray, int32_t ratioRegionNum, baseCov_t *baseCovArray, int32_t printFlag);
short determineSVMisjoin(queryMargin_t *queryMargin, ratioRegion_t *ratioRegionArray, int32_t ratioRegionNum, int32_t subRegSize, query_t *queryItem);
short determineSVQueryIndel(queryIndel_t *queryIndel, ratioRegion_t *ratioRegionArray, int32_t ratioRegionNum, int32_t subRegSize, query_t *queryItem, readSet_t *readSet);
short computeTotalDisagreeNum(int32_t *totalDisagreeNum, int32_t *totalZeroCovNum, ratioRegion_t *ratioRegionArray, int32_t ratioRegionNum, int32_t headSkipRegNum, int32_t tailSkipRegNum);
short getDiscordantRegNum(int32_t *discordantRegNum, ratioRegion_t *ratioRegionArray, int32_t ratioRegionNum, int32_t headSkipRegNum, int32_t tailSkipRegNum);
short getMultiReadsRegNum(int32_t *multiReadsRegNum, ratioRegion_t *ratioRegionArray, int32_t ratioRegionNum, int32_t headSkipRegNum, int32_t tailSkipRegNum);

// ================= misinfo.cpp ====================
short getMisInfoList(query_t *queryItem, subject_t *subjectArray);
short getMisInfoListBreakpoint(query_t *queryItem, subject_t *subjectArray);
short adjustMisInfoList(query_t *queryItem);
short getMisInfoListInner(query_t *queryItem, subject_t *subjectArray);
short computeStartAlignPos(int32_t *startAlignPosQuery, int32_t *startAlignPosSubject, int32_t startQueryPos, int32_t startSubjectPos, globalValidSeg_t *globalSeg, char *querySeq, char *subjectSeq, int32_t initAlignSize);
short initAlignBuf(char **alignResultArray, char **alignSeq1, char **alignSeq2, int32_t maxSeqLen);
void freeAlignBuf(char **alignResultArray, char **alignSeq1, char **alignSeq2);
short increaseAlignBufSize(char **alignResultArray, char **alignSeq1, char **alignSeq2, int32_t *maxSeqLen);
short computeAlignGapNum(int32_t *gapNum1, int32_t *gapNum2, int32_t *misNum, char **alignResultArray, int32_t overlapLen);
short adjustAlignmentGap(int32_t *gapNum1, int32_t *gapNum2, char **alignResultArray, int32_t overlapLen, int32_t printFlag);
short isValidAlignment(int32_t *validAlignFlag, int32_t *validTailAlignFlag, char **alignResultArray, int32_t overlapLen, int32_t queryLeftShiftLen, int32_t subjectLeftShiftLen, int32_t allowLeftShiftFlag);
short addInnerIndelToMisInfoList(query_t *queryItem, int32_t startAlignQueryPos, int32_t startAlignSubjectPos, int32_t subjectID, int32_t strand, char **alignResultArray, int32_t overlapLen, int32_t queryAlignSeqLen, int32_t subjectAlignSeqLen, int32_t queryLeftShiftLen, int32_t subjectLeftShiftLen, int32_t queryRightShiftLen, int32_t subjectRightShiftLen, int32_t gapNumQuery, int32_t gapNumSubject);
short addNewQueryMisInfoNode(query_t *queryItem, int32_t leftSegRow, int32_t rightSegRow, int32_t queryMisInfoType, int32_t queryIndelKind, int32_t gapFlag, int32_t innerFlag, int32_t leftMarginQueryPos, int32_t rightMarginQueryPos, int32_t leftMarginSubjectPos, int32_t rightMarginSubjectPos, int32_t subjectID, int32_t difQuery, int32_t difSubject);
short determineMisInfoSingleQuery(query_t *queryItem, subject_t *subjectArray, readSet_t *readSet);
short computeMisjoinSingleQuery(query_t *queryItem, baseCov_t *baseCovArray, subject_t *subjectArray, readSet_t *readSet);
short determineQueryMis(query_t *queryItem);
short outputMisInfoList(query_t *queryItem);

// ================= correct.cpp ====================
short correctMisassQueries(queryMatchInfo_t *queryMatchInfoSet);
short correctSingleMisassQuery(query_t *queryItem, subject_t *subjectArray);
short getStartEndQueryPosMisCor(int32_t *startQueryPos, int32_t *endQneryPos, misInfo_t *startMisInfo, misInfo_t *endMisInfo, misInfo_t *processedMisInfo, query_t *queryItem);
short computeNewSeqLen(int32_t *newSeqLen, misInfo_t *startMisInfo, misInfo_t *endMisInfo, misInfo_t *processedMisInfo, query_t *queryItem);
short generateNewSeqNode(query_t *queryItem, char **newSeq, int32_t newSeqLen, misInfo_t *startMisInfo, misInfo_t *endMisInfo, misInfo_t *processedMisInfo);
short fillNewBases(query_t *queryItem, char *newSeq, int32_t newSeqLen, misInfo_t *startMisInfo, misInfo_t *endMisInfo, misInfo_t *processedMisInfo, subject_t *subjectArray);
short outputNewSeqInfo(query_t *queryItem);

// ================= misReg.cpp ====================
short extractMisReg(queryMatchInfo_t *queryMatchInfoSet);
short extractMisRegSingleQuery(query_t *queryItem, subject_t *subjectArray);
short addMisassSeqNodeToMisInfoNode(misInfo_t *misInfo, int32_t misassKind, int32_t subjectID, int32_t startQueryPos, int32_t endQueryPos, int32_t startSubjectPos, int32_t endSubjectPos);
short validateMisRegQueryEnd(queryMatchInfo_t *queryMatchInfoSet);

// ================= circosData.cpp ====================
short generateCircosData(queryMatchInfo_t *queryMatchInfoSet, readSet_t *readSet);
short generateCircosQueryData(char *queryFileCircos, queryMatchInfo_t *queryMatchInfoSet);
short fillBaseCovCircosDataSingleQuery(FILE *fpCovData, FILE *fpZeroCov, char *queryLabel, baseCov_t *baseCovArray, int32_t arraySize);
short generateCircosDisNumSingleQuery(FILE *fpQueryDisNum, char *queryLabel, baseCov_t *baseCovArray, int32_t arraySize);
short generateCircosHeatmapAndRatioDataSingleQuery(FILE *fpDiscorRatio, FILE *fpMultiRatio, char *queryLabel, query_t *queryItem, readSet_t *readSet);
short initRatioRegArrayCircos(ratioRegion_t **ratioRegionArray, int32_t *ratioRegionNum, int32_t subRegSize, query_t *queryItem);
short outputCircosHeatmapSingleQuery(FILE *fpDiscorRatio, FILE *fpMultiRatio, char *queryLabel, ratioRegion_t *ratioRegionArray, int32_t ratioRegionNum);
short generateResultCircosDataSingleQuery(FILE *fpResult, char *queryLabel, query_t *queryItem);

// ================= util.cpp ====================
vector<string> split(const string& s, const string& delim);
bool isFileExist(const string &filename);
int copySingleFile(const string &infilename, ofstream &outfile);
short outputLinkedSegments(querySubject_t *pQuerySubject, int headRowSegmentLinkArray, int tailRowSegmentLinkArray, int itemNumSegmentLinkArray, segmentLink_t *segmentLinkArray, matchItem_t *matchItemArray);
short outputQueryMatchInfoText(char *tmpQueryMatchFile, query_t *queryArray, matchItem_t *matchItemArray, int64_t itemNumQueryArray, subject_t *subjectArray);
short checkRefCoveredRatio(metrics_t *queryMetrics, subject_t *subjectArray, int64_t itemNumSubjectArray, query_t *queryArray, int64_t itemNumQueryArray);
short outputReadseqInReadset(char *outfile, readSet_t *readSet);
short outputGapRegInQueries(queryMatchInfo_t *queryMatchInfoSet);
short outputMatchResults(alignMatchItem_t *matchResultArray, int32_t itemNum);

#endif /* METHODS_H_ */
