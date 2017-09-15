#include "yolo.h"
#include "network.h"
#include "parser.h"
#include "stdio.h"
#include "utils.h"
#include <sys/time.h>
#include "image.h"
#include "box.h"
#include "pthread.h"
#include "region_layer.h"

static char **names;
static image **alphabet;
static int classes;

static float **probs;
static box *boxes;
static network net;
static image in[NUM_THREADS]   ;
static image in_s[NUM_THREADS] ;
static image det[NUM_THREADS]  ;
static image det_s[NUM_THREADS];
static image disp[NUM_THREADS] ;
static CvCapture * cap;
static float fps = 0;
static float thresh = 0;

static int  video_finish_flag = 0;

pthread_mutex_t sum_mutex_fetch;
pthread_mutex_t sun_mutex_in;
pthread_mutex_t sum_mutex_det;
pthread_mutex_t sum_mutex_det_s;

static SeqQueue_image IN;
static SeqQueue_image IN_S;
static SeqQueue_image DET;
static SeqQueue_image DET_S;
static SeqQueue_image SHOW;

double get_wall_time()
{
    struct timeval time;
    if (gettimeofday(&time,NULL)){
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

void *fetch_in_thread(void *ptr)
{
    pthread_mutex_lock( &sum_mutex_fetch );
    IplImage* src = cvQueryFrame(cap);
    int posFrames =  (int) cvGetCaptureProperty(cap, CV_CAP_PROP_POS_FRAMES);
    pthread_mutex_unlock( &sum_mutex_fetch );
    IplImage *desc;
    CvSize sz;
    image im;
    image im_s;
    if(src){
        sz.width = net.w;
        sz.height = net.h;
//        pthread_mutex_unlock( &sum_mutex_fetch );
        desc = cvCreateImage(sz,src->depth,src->nChannels);
//#ifdef GPU
//        cvResize_gpu(src,desc);
//#else
        cvResize(src,desc,CV_INTER_LINEAR);
//        pthread_mutex_unlock( &sum_mutex_fetch );
//#endif
        im = ipl_to_image(src);
        im_s = ipl_to_image(desc);
        cvReleaseImage(&desc);
        rgbgr_image(im);
        rgbgr_image(im_s);

        pthread_mutex_lock( &sun_mutex_in );
        EnterQueue(&IN,im);
        EnterQueue(&IN_S,im_s);
        pthread_mutex_unlock( &sun_mutex_in );
//        pthread_mutex_unlock( &sum_mutex_fetch );
    }else{
        video_finish_flag = 1;
//        pthread_mutex_unlock( &sum_mutex_fetch );
    }
    return 0;
}

void *detect_in_thread(void *ptr)
{
    float nms = .4;
    pthread_mutex_lock( &sum_mutex_det_s );
    layer l = net.layers[net.n-1];

    image det_s_tmp;
    DeleteQueue(&DET_S,&det_s_tmp);
    float *X = det_s_tmp.data;
    float *prediction = network_predict(net, X);//network_predict
    l.output = prediction;

    free_image(det_s_tmp);
    pthread_mutex_unlock( &sum_mutex_det_s );
    if (l.type == REGION){
        get_region_boxes(l, det_s_tmp.w, det_s_tmp.h, net.w, net.h, thresh, probs, boxes);
    } else {
        error("Last layer must produce detections\n");
    }
    if (nms > 0) do_nms(boxes, probs, l.w*l.h*l.n, l.classes, nms);
//    printf("\033[2J");
//    printf("\033[1;1H");
//    printf("\nFPS:%.1f\n",fps);
//    printf("Objects:\n\n");

    pthread_mutex_lock( &sum_mutex_det );
    image det_tmp;
    DeleteQueue(&DET,&det_tmp);
    draw_detections(det_tmp, l.w*l.h*l.n, thresh, boxes, probs, names, alphabet, classes);
    pthread_mutex_unlock( &sum_mutex_det );

    return 0;
}

void *show_in_thread(void *ptr)
{
//   double before_5 = get_wall_time();
   image disp_1;
   DeleteQueue(&SHOW,&disp_1);
   show_image(disp_1, "Demo");
   cvWaitKey(1);
   free_image(disp_1);

//   double after_5 = get_wall_time();
//   printf("\n...........SHOW_time:%f\n",(after_5 - before_5)/NUM_THREADS);
   return 0;
}

void yolo(char *cfgfile, char *weightfile, float thresh_obj, int cam_index, const char *filename, int classes_obj, char **names_obj, int fullscreen)
{
    thresh = thresh_obj;
    alphabet = load_alphabet();
    classes = classes_obj;
    names = names_obj;
    printf("YOLO_SYOUNG\n");
    net = parse_network_cfg(cfgfile);//parse network cfg
    if(weightfile){
        load_weights(&net, weightfile);//load weights that are trained
    }

    if(filename){
        printf("video file: %s\n", filename);
        cap = cvCaptureFromFile(filename);
    }else{
        cap = cvCaptureFromCAM(cam_index);
    }

    if(!cap) error("Couldn't connect to webcam.\n");

    layer l = net.layers[net.n-1];
    boxes = (box *)calloc(l.w*l.h*l.n, sizeof(box));
    probs = (float **)calloc(l.w*l.h*l.n, sizeof(float *));

    int j;
    for(j = 0; j < l.w*l.h*l.n; ++j) probs[j] = (float *)calloc(l.classes+1, sizeof(float));

    pthread_t fetch_thread[NUM_THREADS];
    pthread_t detect_thread[NUM_THREADS];
    pthread_t show_thread[NUM_THREADS];

    InitQueue(&IN);
    InitQueue(&IN_S);
    InitQueue(&DET);
    InitQueue(&DET_S);
//    InitQueue(&SHOW);

    int i;
    for( i = 0; i < NUM_THREADS; ++i ){
        fetch_in_thread(0);
       }

    for( i = 0; i < NUM_THREADS; ++i ){
        DeleteQueue(&IN,&in[i]);
        DeleteQueue(&IN_S,&in_s[i]);
        det[i] = in[i];
        det_s[i] = in_s[i];
       }

    if(QueueEmpty(IN)&&QueueEmpty(IN_S)){
        InitQueue(&IN);
        InitQueue(&IN_S);
    }

    for( i = 0; i < NUM_THREADS; ++i ){
        fetch_in_thread(0);
       }

    for( i = 0; i < NUM_THREADS; ++i ){
        DeleteQueue(&IN,&in[i]);
        DeleteQueue(&IN_S,&in_s[i]);
        disp[i] = det[i];
        det[i] = in[i];
        det_s[i] = in_s[i];
       }

    for( i = 0; i < NUM_THREADS; ++i ){
        EnterQueue(&DET,det[i]);
        EnterQueue(&DET_S,det_s[i]);
//        EnterQueue(&SHOW,disp[i]);
        }

    pthread_mutex_init( &sum_mutex_fetch, NULL ); //initial the lock of pthread
    pthread_mutex_init( &sun_mutex_in, NULL ); //initial the lock of pthread
    pthread_mutex_init( &sum_mutex_det, NULL ); //initial the lock of pthread
    pthread_mutex_init( &sum_mutex_det_s, NULL ); //initial the lock of pthread

    cvNamedWindow("Demo", CV_WINDOW_NORMAL);
    if(fullscreen){
        cvSetWindowProperty("Demo", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
    } else {
        cvMoveWindow("Demo", 0, 0);
        cvResizeWindow("Demo", 1352, 1013);
    }

    double before = get_wall_time();
    float fps_average = 0;
    float fps_average_tmp = 0;
    int count = 0;
    char filename_out[1024];
    int count_image_out =0;
    while(1){
        count++;
        count_image_out++;
        InitQueue(&IN);
        InitQueue(&IN_S);
        double before_1 = get_wall_time();
        for( i = 0; i < NUM_THREADS; ++i ){
             pthread_create(&detect_thread[i], 0, detect_in_thread, 0);
             pthread_create(&fetch_thread[i], 0, fetch_in_thread, 0);
        }

        int q;
        for( q = 0; q < NUM_THREADS; ++q ){
//            pthread_join(show_thread[q], 0);
 //           if(disp[q].data!=NULL)
//            {
              show_image(disp[q], "Demo");
              cvWaitKey(1);
//              sprintf(filename_out,"/home/cpk/Desktop/yolov2_revise/image_out/%06d",count_image_out);
//              save_image(disp[q], filename_out);
                free_image(disp[q]);
//            }
        }

        int j;
        double after_det_2;
        for( j = 0; j < NUM_THREADS; ++j ){
             pthread_join(detect_thread[j], 0);
        }

        after_det_2 = get_wall_time();
        printf("\nDetection_time:%f\n",(after_det_2 - before_1)/NUM_THREADS);

        int k;
        for( k = 0; k < NUM_THREADS; ++k ){
           pthread_join(fetch_thread[k], 0);
        }

        if(video_finish_flag == 1){
            break;
        }

        double after_1 = get_wall_time();
        printf("\nResize_time:%f\n",(after_1 - before_1)/NUM_THREADS);

        for( i = 0; i < NUM_THREADS; ++i ){
            DeleteQueue(&IN,&in[i]);
            DeleteQueue(&IN_S,&in_s[i]);
            disp[i] = det[i];
            det[i] = in[i];
            det_s[i] = in_s[i];
            }

            InitQueue(&DET);
            InitQueue(&DET_S);

        for( i = 0; i < NUM_THREADS; ++i ){
            EnterQueue(&DET,det[i]);
            EnterQueue(&DET_S,det_s[i]);
//            EnterQueue(&SHOW,disp[i]);
            }

        double after = get_wall_time();
        float curr = 1./(after - before);
        fps = curr * NUM_THREADS;
        float fps_average_m = 1/curr;
        fps_average = fps_average + fps_average_m;
        if(count == AVERAGE){
            fps_average_tmp = 1/fps_average*NUM_THREADS*count;
            count = 0;
            fps_average = 0;
        }
        printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>fps_average:%f\n",fps_average_tmp);
        printf("\nFPS_time:%f\n",1/fps);
        before = after;
    }
    pthread_mutex_destroy( &sum_mutex_fetch ); //destory the lock of pthread
    pthread_mutex_destroy( &sun_mutex_in );
    pthread_mutex_destroy( &sum_mutex_det ); //destory the lock of pthread
    pthread_mutex_destroy( &sum_mutex_det_s);

    free_network(net);
}

// 初始化队列
void InitQueue(SeqQueue_image *SQ){
     SQ->front = 0;
     SQ->rear = 0;
}

// 判断队列是否为空
int QueueEmpty(SeqQueue_image SQ){
    // 队头坐标与队尾坐标相等时，即为空队列
    if(SQ.front == SQ.rear){
         return 1;
    }else{
        return 0;
    }
}

// 入队操作
int EnterQueue(SeqQueue_image *SQ,image e){
    // 边界判断，假如队列满了不能入队
    if(SQ->rear == QueueSize){
         printf("队列已满，不能入队.\n");
         return 0;
    }
    // 新元素入队，需要将队尾指针往后移动
    SQ->queue[SQ->rear++] = e;
    return 1;
}

// 出队操作
int DeleteQueue(SeqQueue_image *SQ,image *e){
    // 边界判断，假如队列空了不能出队
    if(SQ->front == SQ->rear){
         printf("队列已空，不能出队.\n");
         return 0;
    }
    // 元素出队，需要将队头指针往后移动
    *e = SQ->queue[SQ->front++];
    return 1;
}
