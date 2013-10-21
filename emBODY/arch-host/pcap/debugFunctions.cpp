/*
 * Copyright (C) 2012 iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Alberto Cardellino
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "header.h"
#include "debugFunctions.h"


DebugData           emsToPc104[BOARD_NUM]    = {0};
DebugData           pc104ToEms[BOARD_NUM]    = {0};

FILE                *outFile                = stdout;




int ipL2boardNum(in_addr_t addr)
{
    int board;

    switch(addr)
    {
        case eb1_hex_addr:
            board = 1;
            break;

        case eb2_hex_addr:
            board = 2;
            break;

        case eb3_hex_addr:
            board = 2;
            break;

        case eb4_hex_addr:
            board = 4;
            break;

        case eb5_hex_addr:
            board = 5;
            break;

        case eb6_hex_addr:
            board = 6;
            break;

        case eb7_hex_addr:
            board = 7;
            break;

        case eb8_hex_addr:
            board = 8;
            break;

        case eb9_hex_addr:
            board = 9;
            break;

        case pc104_hex_addr:
            board = 104;
            break;

        default:
            printf("no board 0x%X\n", addr);
    }

//  do_real_check_positons(board, pkt, pkt_size);
    return  board;
}




void check_received_pkt(const in_addr sender_addr, const in_addr destin_addr, void *pkt, uint16_t pkt_size, const timeval *sniffTime)
{
    int sender = ipL2boardNum(sender_addr.s_addr);
    int destin = ipL2boardNum(destin_addr.s_addr);

    if(sender == 104)
    {
        check_seqNum(destin, (EOropframeHeader_t *) pkt, &pc104ToEms[destin], sniffTime);
    }
    else
    {
        check_seqNum(sender, (EOropframeHeader_t *) pkt, &emsToPc104[sender], sniffTime);
    }
}


bool do_real_check_positons(int board, void *pkt, uint16_t size)
{
    static int prev = 0;
    static int print=0;
    int parsed = 0;
    int16_t *tmp_datasize;
    int16_t datasize;
    uint8_t *data = (uint8_t *)pkt;

    print++;


//    if(print == 200)
    {
//      fprintf(outFile,"RopFrame Header = ");
//      for(int i =0; i<4; i++)
//          fprintf(outFile,"%02x",data[i]);

        int16_t *nrops = (int16_t *)&data[6];

//      printf("\n");
//      for(int i =0; i<8; i++)
//          printf("%02x",data[8+i]);

        uint32_t *num=(uint32_t *) &data[8];

        if(*num != prev+1)
        {
            printf("\nMissing something!!!! (%d)\n", prev+1);
        }

        prev = *num;
//      fprintf(outFile,"\nnrops %d\n", *nrops);
        parsed = 16;

//      for(int n=0; n<*nrops; n++)
////        while(parsed < size)
//      {
//          for(int i =0; i<6; i++, parsed++)
//              fprintf(outFile,"%02x-", data[parsed]);
//
//          tmp_datasize = (int16_t*)&data[parsed];
//          datasize = *tmp_datasize;
//          fprintf(outFile," size = %d (%0X%0X)\n\t", datasize, data[parsed], data[parsed+1]);
//          parsed += 2;
//
//          int fill=datasize%4;
//          int howmuch=0;
//          if(fill!=0)
//              fill=4-fill;
//
//              howmuch = (datasize) + fill;
////            printf(" howmuch %d\n", howmuch);
//
//          for(int i =0; i<howmuch; i++, parsed++)
//          {
//              if( (i%4) == 0)
//                  fprintf(outFile, "-");
//              fprintf(outFile, "%02x", data[parsed]);
//          }
//          /*
//          if(*datasize == 32) // E' un joint status
//          {
//              int32_t *pos = (int32_t*) &data[parsed];
//              printf(" pos = 0X%04X", *pos);
//              //for(int i=0; i<4; i++)
//              //  printf("%02X", data[parsed +i]);
//
//              printf(")");
//          }*/
//
//          fprintf(outFile, "\n");
//          parsed+= *datasize;
//      }
//      fprintf(outFile, "END of packet\n");
        print=0;
    }
    return true;
}

void check_seqNum(int boardNum, EOropframeHeader_t *pkt, DebugData *debugData, const struct timeval *pcapTime)
{
    struct timeval diffTime;
    struct timeval sniffTime(*pcapTime);

    if(debugData->totPcktRecv == 0)
    {
        memcpy(&(debugData->previousPkt), pkt, sizeof(EOropframeHeader_t) );
        debugData->totPcktRecv++;

        printf("First packet from board %d; seq num is %lu\n", boardNum, pkt->sequencenumber);
    }
    else
    {
        if( pkt->sequencenumber != (debugData->previousPkt.sequencenumber + 1))
        {
            fprintf(outFile,"missing packet for board %d: old = %lu, new = %lu\n", boardNum, debugData->previousPkt.sequencenumber, pkt->sequencenumber);
            debugData->errors.missing++;
        }

        timeval_subtract(&diffTime, &sniffTime, &(debugData->sniffTimePrev));

        if(diffTime.tv_usec >= SOGLIA)
        {
//          fprintf(outFile, "time between packets %06d.%06d\n", diffTime[board].tv_sec, diffTime[board].tv_usec);
            debugData->errors.late++;
        }

        if( 0 == (debugData->totPcktRecv % 20000))
        {
            printf(">periodic check:\tprogNum[%d] = %lu\n", boardNum, debugData->previousPkt.sequencenumber);
            print_data(boardNum, debugData);
        }

        // update stored data for previous msg
        memcpy(&(debugData->previousPkt), pkt, sizeof(EOropframeHeader_t) );
        debugData->sniffTimePrev = sniffTime;
        debugData->totPcktRecv++;
    }
}

void print_data(int board, DebugData *data)
{
    printf("Errors for board %d\n",board);
    printf("\tmissing = %d over %d\n", data->errors.missing, data->totPcktRecv);
    printf("\tlate    = %d over %d\n\n", data->errors.late, data->totPcktRecv);
}

void printEverything(void)
{
    printf("\nPrinting pc104 receiving data\n\n");
    for(int b=1; b< BOARD_NUM; b++)
    {
        print_data(b, &emsToPc104[b]);
    }

    printf("\nPrinting pc104 sending data\n");
    for(int b=1; b< BOARD_NUM; b++)
    {
        print_data(b, &pc104ToEms[b]);
    }
}

#ifdef _WIN32_
// Definition of a gettimeofday function

int gettimeofday(struct timeval *tv, struct timezone *tz)
{
// Define a structure to receive the current Windows filetime
    FILETIME ft;

// Initialize the present time to 0 and the timezone to UTC
    unsigned __int64 tmpres = 0;
    static int tzflag = 0;

    if(NULL != tv)
    {
        GetSystemTimeAsFileTime(&ft);

// The GetSystemTimeAsFileTime returns the number of 100 nanosecond
// intervals since Jan 1, 1601 in a structure. Copy the high bits to
// the 64 bit tmpres, shift it left by 32 then or in the low 32 bits.
        tmpres |= ft.dwHighDateTime;
        tmpres <<= 32;
        tmpres |= ft.dwLowDateTime;

// Convert to microseconds by dividing by 10
        tmpres /= 10;

// The Unix epoch starts on Jan 1 1970.  Need to subtract the difference
// in seconds from Jan 1 1601.
        tmpres -= DELTA_EPOCH_IN_MICROSECS;

// Finally change microseconds to seconds and place in the seconds value.
// The modulus picks up the microseconds.
        tv->tv_sec = (long)(tmpres / 1000000UL);
        tv->tv_usec = (long)(tmpres % 1000000UL);
    }

    if(NULL != tz)
    {
        if(!tzflag)
        {
            _tzset();
            tzflag++;
        }

// Adjust for the timezone west of Greenwich
        tz->tz_minuteswest = _timezone / 60;
        tz->tz_dsttime = _daylight;
    }

    return 0;
}
#endif
