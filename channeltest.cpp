#include "streamdevice.h"
#include "channel.h"
#include "helpers.h"
#include "memory.h"
#include <memory>

//Build command = g++ -o ctest.out channeltest.cpp stdiodevice.cpp channel.cpp subchannel.cpp memory.cpp

int main(){
    std::shared_ptr<memory> mem = std::make_shared<memory>(10);
    mem->writeByte(0,(byte)'H',0);
    mem->writeByte(1,(byte)'e',0);
    mem->writeByte(2,(byte)'l',0);
    mem->writeByte(3,(byte)'l',0);
    mem->writeByte(4,(byte)'o',0);
    mem->writeByte(5,(byte)',',0);
    mem->writeByte(6,(byte)' ',0);
    mem->writeByte(7,(byte)'W',0);
    mem->writeByte(8,(byte)'o',0);
    mem->writeByte(9,(byte)'r',0);
    mem->writeByte(10,(byte)'l',0);
    mem->writeByte(11,(byte)'d',0);
    mem->writeByte(12,(byte)'!',0);
    mem->writeByte(13,(byte)'\0',0);
    //std::cout << "test0" << std::endl;
    doubleword ccw = 0x0D0000000000000E;
    //std::cout << 80 % (1 << 11);
    mem->writeDoubleword(80,ccw,0); //Write CCW
    //std::cout << "testCAW" << std::endl;
    word caw = 0x00000050;
    mem->writeWord(72,caw,0);
    
    stdioDevice testDev;

    channel testChannel(mem,0);
    //std::cout << "test1" << std::endl;
    testChannel.addSubchannel(0);
    //std::cout << "test2" << std::endl;
    testChannel.addDevice({0,0},&testDev);
    //std::cout << "test3" << std::endl;
    testChannel.startIO({0,0});
}