#include "inc/stdinc.h"
#include "inc/global.h"


int main(int argc, char **argv)
{
	if(parseCommandParasAndExe(argc, argv)==ERROR)
	{
		printf("line=%d, In %s(), cannot parse the command parameters, error!\n", __LINE__, __func__);
		return FAILED;
	}

	return SUCCESSFUL;
}

/**
 * Parse the input parameters.
 *  @return:
 *  	If succeeds, return SUCCESSFUL;
 *  	if the job is executed and failed, return ERROR;
 *  	otherwise, return FAILED.
 */
short parseCommandParasAndExe(int argc, char **argv)
{
	int32_t i, operationMode;
	char configFilePara[256];
	char outputDirPara[256];
	int32_t singleCellFlagPara, minQueryLenPara, threadNumPara;
	double minIdentityPercentPara;
	int32_t indelSizeThresPara;
	vector<string> no_arg_opt_vec;

#if __WORDSIZE!=64
	printf("Please compile and run the tool on x86_64 Linux system.\n");
	return FAILED;
#endif

	if(argc==1)
	{
		if(showUsageInfo()==FAILED)
		{
			printf("line=%d, In %s(), cannot show the usage information, error!\n", __LINE__, __func__);
			return FAILED;
		}
	}else if(argc==2)
	{
		i = 1;
		while(i<argc)
		{
			if(strcmp(argv[i], "all")==0 or strcmp(argv[i], "merge")==0 or strcmp(argv[i], "metrics")==0)
			{
				if(showUsageInfo()==FAILED)
				{
					printf("line=%d, In %s(), cannot show the usage information, error!\n", __LINE__, __func__);
					return FAILED;
				}
			}else if(strcmp(argv[i], "gen-conf")==0){ // generate configuration file
				if(generateConfigFile(CONFIGFILENAME_DEFAULT)==FAILED){
					printf("line=%d, In %s(), cannot create configuration file '%s', error!\n", __LINE__, __func__, CONFIGFILENAME_DEFAULT);
					return FAILED;
				}else{
					cout << "Configuration file '" << CONFIGFILENAME_DEFAULT << "' is generated successfully." << endl;
					return SUCCESSFUL;
				}
			}else if(strcmp(argv[i], "-h")==0 || strcmp(argv[i], "-help")==0)
			{
				if(showUsageInfo()==FAILED)
				{
					printf("line=%d, In %s(), cannot show the usage information , error!\n", __LINE__, __func__);
					return FAILED;
				}
			}else
			{
				if(argv[i][0]=='-')
				{
					printf("%s : unknown argument\n", argv[i]);
					printf("Please use -h or -help for more information.\n");
				}else{
					//no_arg_opt_vec.push_back(argv[i]);
//					printf("%s : invalid argument\n", argv[i]);
//					printf("Please use -h or -help for more information.\n");
				}
				return FAILED;
			}

			i ++;
		}

		return FAILED;
	}else
	{
		// reset the parameters
		operationMode = -1;
		configFilePara[0] = '\0';
		outputDirPara[0] = '\0';
		singleCellFlagPara = 0;
		minQueryLenPara = 0;
		minIdentityPercentPara = 0;
		threadNumPara = 0;
		indelSizeThresPara = 0;

		if(strcmp(argv[1], "all")==0)
		{
			operationMode = OPERATION_MODE_ALL;
		}else if(strcmp(argv[1], "merge")==0)
		{
			operationMode = OPERATION_MODE_MERGE;
		}else if(strcmp(argv[1], "metrics")==0)
		{
			operationMode = OPERATION_MODE_METRICS;
		}else if(strcmp(argv[1], "misass")==0)
		{
			operationMode = OPERATION_MODE_MISASS;
		}else if(strcmp(argv[1], "gen-conf")==0)
		{
			operationMode = OPERATION_MODE_GENCONF;
		}else
		{
			if(strcmp(argv[1], "-h")==0 || strcmp(argv[1], "-help")==0)
			{
				if(showUsageInfo()==FAILED)
				{
					printf("line=%d, In %s(), cannot show the usage information, error!\n", __LINE__, __func__);
					return FAILED;
				}
			}else
			{
				if(argv[1][0]=='-')
				{
					printf("%s : unknown argument\n", argv[1]);
					printf("Please use -h or -help for more information.\n");
				}else
				{
					printf("%s : invalid argument\n", argv[1]);
					printf("Please use -h or -help for more information.\n");
				}
			}

			return FAILED;
		}

		i = 2;
		while(i<argc){
			if(strcmp(argv[i], "-o")==0 || strcmp(argv[i], "-out")==0)
			{
				if(i+1<argc)
				{
					if(argv[i+1][0]!='-')
					{
						strcpy(outputDirPara, argv[i+1]);
						outputDirPara[strlen(argv[i+1])] = '\0';
					}else
					{
						printf("Exception: please specify the correct output directory.\n");
						return FAILED;
					}
				}else
				{
					printf("Exception: please specify the output directory.\n");
					return FAILED;
				}
				i += 2;
			}else if(strcmp(argv[i], "-m")==0)
			{
				if(i+1<argc)
				{
					if(argv[i+1][0]!='-')
					{
						minQueryLenPara = atol(argv[i+1]);
						if(minQueryLenPara<=0)
						{
							printf("Exception: please specify the correct minimal query length.\n");
							return FAILED;
						}
					}else
					{
						printf("Exception: please specify the correct minimal query length.\n");
						return FAILED;
					}
				}else
				{
					printf("Exception: please specify the minimal query length.\n");
					return FAILED;
				}
				i += 2;
			}else if(strcmp(argv[i], "-pt")==0)
			{
				if(i+1<argc)
				{
					if(argv[i+1][0]!='-')
					{
						minIdentityPercentPara = atof(argv[i+1]);
						if(minIdentityPercentPara<=0 || minIdentityPercentPara>1.0)
						{
							printf("Exception: please specify the correct minimal aligned identity percent value, error and exit!\n");
							return FAILED;
						}
					}else
					{
						printf("Exception: please specify the correct minimal aligned identity percent value.\n");
						return FAILED;
					}
				}else
				{
					printf("Exception: please specify the minimal aligned identity percent value.\n");
					return FAILED;
				}
				i += 2;
			}else if(strcmp(argv[i], "-t")==0)
			{
				if(i+1<argc)
				{
					if(argv[i+1][0]!='-')
					{
						threadNumPara = atol(argv[i+1]);
						if(threadNumPara<=0)
						{
							printf("Exception: please specify the correct number of threads.\n");
							return FAILED;
						}
					}else
					{
						printf("Exception: please specify the correct number of threads.\n");
						return FAILED;
					}
				}else
				{
					printf("Exception: please specify the number of threads.\n");
					return FAILED;
				}
				i += 2;
			}else if(strcmp(argv[i], "-i")==0)
			{
				if(i+1<argc)
				{
					if(argv[i+1][0]!='-')
					{
						indelSizeThresPara = atol(argv[i+1]);
						if(indelSizeThresPara<=0)
						{
							printf("Exception: please specify the minimal indel size.\n");
							return FAILED;
						}
					}else
					{
						printf("Exception: please specify the correct minimal indel size.\n");
						return FAILED;
					}
				}else
				{
					printf("Exception: please specify the minimal indel size.\n");
					return FAILED;
				}
				i += 2;
			}else if(strcmp(argv[i], "-sc")==0)
			{
				if(i+1<argc)
				{
					if(argv[i+1][0]!='-')
					{
						singleCellFlagPara = atol(argv[i+1]);
						if(singleCellFlagPara!=0 && singleCellFlagPara!=1)
						{
							printf("Exception: please specify the correct single cell data flag if the paired-end reads was sequenced by the single cell technology.\n");
							return FAILED;
						}
					}else
					{
						printf("Exception: please specify the correct single cell data flag.\n");
						return FAILED;
					}
				}else
				{
					printf("Exception: please specify the single cell data flag.\n");
					return FAILED;
				}
				i += 2;
			}else if(strcmp(argv[i], "-h")==0 || strcmp(argv[i], "-help")==0)
			{
				if(showUsageInfo()==FAILED)
				{
					printf("line=%d, In %s(), cannot show the usage information, error!\n", __LINE__, __func__);
					return FAILED;
				}

				return FAILED;
			}else{
//				if(i+1==argc){ // configuration file
//					strcpy(configFilePara, argv[i]);
//					outputDirPara[strlen(argv[i])] = '\0';
//					i ++;
//				}else{
					if(argv[i][0]=='-'){
						printf("%s : unknown argument\n", argv[i]);
						printf("Please use -h or -help for more information.\n");
						return FAILED;
					}else{
						no_arg_opt_vec.push_back(argv[i]);
						i++;
//						printf("%s : invalid argument\n", argv[i]);
//						printf("Please use -h or -help for more information.\n");
					}
//				}
			}
		}

		// process no_arg_opt_vec: config file
		if(no_arg_opt_vec.size()==1){
			strcpy(configFilePara, no_arg_opt_vec.at(0).c_str());
			configFilePara[no_arg_opt_vec.at(0).size()] = '\0';
		}else if(no_arg_opt_vec.size()>1){
			for(i=0; i<(int32_t)no_arg_opt_vec.size(); i++)
				printf("%s : invalid argument\n", argv[i]);
			printf("Please use -h or -help for more information.\n");
			return FAILED;
		}

		if(strlen(configFilePara)==0){
			if(operationMode==OPERATION_MODE_GENCONF){
				strcpy(configFilePara, CONFIGFILENAME_DEFAULT);
				configFilePara[strlen(CONFIGFILENAME_DEFAULT)] = '\0';
			}else{
				printf("Exception: please specify the correct configuration file.\n");
				return FAILED;
			}
		}

		// check whether the parameter is valid
		if(operationMode!=OPERATION_MODE_ALL and operationMode!=OPERATION_MODE_MERGE and operationMode!=OPERATION_MODE_METRICS and operationMode!=OPERATION_MODE_MISASS and operationMode!=OPERATION_MODE_GENCONF)
		{
			printf("Exception: please specify the correct operations: all, merge, metrics, misass or gen-conf\n");
			return FAILED;
		}

		if(operationMode!=OPERATION_MODE_GENCONF){
			// begin to do the job
			if(computeGlobalMetrics(operationMode, outputDirPara, configFilePara, minQueryLenPara, minIdentityPercentPara, threadNumPara, indelSizeThresPara, singleCellFlagPara)==FAILED)
			{
				printf("line=%d, In %s(), cannot compute the metrics, error!\n", __LINE__, __func__);
				return ERROR;
			}
		}else{ // create configuration file
			if(generateConfigFile(configFile)==FAILED){
				printf("line=%d, In %s(), cannot show the usage information, error!\n", __LINE__, __func__);
				return FAILED;
			}else{
				cout << "Configuration file '" << configFile << "' is generated successfully." << endl;
				return SUCCESSFUL;
			}
		}
	}

	return SUCCESSFUL;
}

/**
 * Show the usage information.
 *  @return:
 *  	If succeeds, return SUCCESSFUL; otherwise return FAILED.
 */
short showUsageInfo()
{
	// version information
	printf("misFinder: %s\n", MISFINDER_VERSION_STR);
	printf("Released : %s\n", MISFINDER_RELEASE_DATE_STR);

	printf("\nUsage: mf <command> [options] <CONF_FILE>\n\n");

	printf("Description:\n");
	printf("    CONF_FILE   Configuration file (required)\n\n");

	printf("Commands:\n");
	printf("    metrics     Compute the assembly metrics\n");
	printf("    misass      Compute mis-assemblies\n");
	printf("    all         Do all the above in turn\n");
	printf("    gen-conf    Create configuration file\n\n");

	printf("Options:\n");
	printf("    -t <INT>        The number of threads. Default is the number of CPU cores.\n");
	printf("    -o|-out <STR>   Output directory. [%s]\n", OUTPUT_DIR_DEFAULT);
	printf("    -v              Show version information.\n");
	printf("    -h|-help        Show this help information and exit.\n\n");

	printf("Command Options:\n");
	printf("  1) metrics    -- compute the metrics:\n");
	printf("    -m <INT>        The minimal query length (bp). [%d]\n", MIN_QUERY_LEN_THRES);
	printf("    -pt <FLOAT>     The minimal identity percentage for matched queries and \n"
		   "                    matched segments. [%g]\n", MATCHED_PERCENT_THRES);

	printf("  2) misass     -- compute mis-assemblies:\n");
	printf("    -i <INT>        Minimal indel size (bp). [%d]\n", INDEL_SIZE_DEFAULT);
	printf("    -sc <INT>       Single-cell paired-end data flag. [0]\n"
		   "                    0: standard genomic DNA prepared from culture;\n"
		   "                    1: single-cell data.\n");

	printf("  3) gen-conf   -- create configuration file in current directory:\n\n");

	printf("Example:\n");
	printf("  # detect mis-assemblies based on the specified information in 'config' file \n");
	printf("  $ mf all -o output config\n\n");

	printf("  # just compute assembly metrics\n");
	printf("  $ mf metrics -o output config\n\n");

	printf("  # create the 'config' file in current directory\n");
	printf("  $ mf gen-conf config\n");

	return SUCCESSFUL;
}
