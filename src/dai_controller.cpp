#include <memory>
#include <depthai/depthai.hpp>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h> //v4l2 format commands
#include <linux/v4l2loopback.h>
#include <poll.h>

// api events copied out of v4l2loopback.c source
// this is the listener event for when an app begins streaming from the camera device
#define V4L2_EVENT_PRI_CLIENT_USAGE \
	(V4L2_EVENT_PRIVATE_START + 0x08E00000 + 1)

struct v4l2_event_client_usage {
	__u32 count;
};

// Create the camera device, get the image, stream it to the device
// I would love to seperate the v4l2 device and dai pipeline but there's not much reason too
int dai_run() {

  // 1. CREATING THE DEVICE

  // v4l2loopback device config struct
  v4l2_loopback_config cfg{};
  cfg.output_nr = -1; // sets the device number, -1 is autofill
  strncpy(cfg.card_label, "Oak-D Camera", sizeof(cfg.card_label)-1);
  cfg.announce_all_caps = 0; //exclusive_caps=1, for device recognition
  //device number
  int ctl = open("/dev/v4l2loopback", O_RDWR); 
  if (ctl < 0)
    perror("open ctl");
  int nr=ioctl(ctl, V4L2LOOPBACK_CTL_ADD, &cfg); //creates /dev/video<nr>
  if (nr < 0)
    perror("CTL_ADD");
  if (nr < 0) return -1;

  //declaring video device path
  char path[32];
  snprintf(path, sizeof(path), "/dev/video%d",nr);

  // 2. CREATING A FILE DESCRIPTOR FOR THE DEVICE
  int fd = open(path, O_RDWR);
  
  // setting image format
  v4l2_format fmt{};
  fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
  fmt.fmt.pix.width = 1920;
  fmt.fmt.pix.height = 1080;
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUV420;
  fmt.fmt.pix.sizeimage = 1920*1080*3/2;
  ioctl(fd, VIDIOC_S_FMT, &fmt);
  
  //write a blank frame to advertise as a capture device
  std::vector<uint8_t> blank(1920*1080*3/2, 128);   // U/V = 128 (neutral)
  std::fill_n(blank.begin(), 1920*1080, 0);         // Y = 0 → black
  write(fd, blank.data(), blank.size());
  
  // creating event listener for fd streams
  // sub is reuseable
  v4l2_event_subscription sub{};
  sub.type = V4L2_EVENT_PRI_CLIENT_USAGE;
  // the actual subscriber
  ioctl(fd, VIDIOC_SUBSCRIBE_EVENT, &sub);
  // EVENT LOOP
  while (true) {
    

    // poll/listener for the event(an application subscribes) to change
    pollfd p{fd, POLLPRI, 0};
    if (poll(&p, 1, -1) > 0 && (p.revents & POLLPRI)) {
      // clear just occured event
      v4l2_event ev{}; // event data
      ioctl(fd, VIDIOC_DQEVENT, &ev);
      
      v4l2_event_client_usage u;
      memcpy(&u, ev.u.data, sizeof(u));
      int count = u.count;
      if (count > 0) {

        // 3. CREATING THE DAI STREAM
        // Shared ptr automatically destroys itself when it has no more references. In this case when the function ends
        std::shared_ptr<dai::Device> device = std::make_shared<dai::Device>();

        // Create Pipeline
        dai::Pipeline pipeline(device);

        //Create camera node
        auto cam = pipeline.create<dai::node::Camera>() -> build(dai::CameraBoardSocket::CAM_A);
        
        // Creating the camera stream. DAI auto-outputs to 420p
        std::shared_ptr<dai::MessageQueue> videoQueue = cam->requestOutput({1920,1080}, dai::ImgFrame::Type::YUV420p, dai::ImgResizeMode::STRETCH,30)->createOutputQueue(8,false);

        pipeline.start();

        while (count > 0) {
          //get image frame
          std::shared_ptr<dai::ImgFrame> videoIn=videoQueue->get<dai::ImgFrame>();
          if (videoIn != nullptr) {
            //auto is creating a copy of the object
            auto data = videoIn->getData();
            //writing buffer to stream
            write(fd, data.data(), 1920*1080*3/2);
          }

          // polling for the subscriber count again
          if (poll(&p, 1, 0) > 0 && (p.revents & POLLPRI)) {
            ioctl(fd, VIDIOC_DQEVENT, &ev);
            memcpy(&u, ev.u.data, sizeof(u));
            count = u.count;
          }
        }
        pipeline.stop();
      }
    }
  }
  ioctl(ctl, V4L2LOOPBACK_CTL_REMOVE, nr);
  return 0;
}
