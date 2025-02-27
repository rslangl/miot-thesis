#include "HeartSensorController.h"
#include "CayenneLPP.h"

HeartSensorController::HeartSensorController(PinName output, PinName lo_neg, PinName lo_pos) 
    :
    _output(output),
    _lo_neg(lo_neg),
    _lo_pos(lo_pos)
{
    init();
}

void HeartSensorController::init()
{
    this->_bpm = 60;            
    printf("\r\n [HR] Controller init! \r\n");
}

/**
 * Disclaimer: code taken from https://os.mbed.com/users/maryannionascu/code/HeartRateMonitor//file/ad2548407023/main.cpp/
 * and slightly modified to match this project
 */
void HeartSensorController::read(CayenneLPP* clpp)
{
    float beat              = 0.0;
    float sumIBI            = 0.0;
    float prevIBI           = 1.0;
    float currIBI           = 0.0;
    float pastIBI[10]{0};
    float peak              = 25.0;
    float trough            = 15.0;
    float amp               = 5.0;
    float thresh_low        = 0.5;
    float thresh_upper      = 0.6;
    bool beat_found         = false;
    bool first_beat         = false;
    bool BPM_found          = false;
    uint64_t last_beat_time = 0;
    int beat_count          = 0;

    printf("\r\n [HR] ");

    //__disable_irq();

    _tmr.start();

    // Calculate BPM on-the-fly during a 10 second time window
    while(_tmr.elapsed_time() <= std::chrono::seconds(10))
    {
        beat = _output.read() * 100;
        currIBI =  std::chrono::duration<float>(_tmr.elapsed_time()).count() - last_beat_time;
        
        // Avoid noise by waiting 3/5 of last IBI
        if((beat < thresh_low) && (currIBI > (prevIBI * 3) / 5))
        {
            if(beat < trough)
            {
                trough = beat;
            }
        }
        if((beat > thresh_upper) && (beat > peak))
        {
            peak = beat;
        }
        
        // Signal surges when there is a heart beat
        if((currIBI > 0.25) && (beat > thresh_upper) && (beat_found == false))
        {
            beat_found = true;
            beat_count++;
            thresh_low = beat - 0.7;

            prevIBI = std::chrono::duration<float>(_tmr.elapsed_time()).count() - last_beat_time;
            last_beat_time = std::chrono::duration<float>(_tmr.elapsed_time()).count();
            
            // If first beat found, fill pastIBI vals with this value until more beats are found
            if(first_beat)
            {
                first_beat = false;
                
                for(int i = 0; i < 10; i++)
                {
                    pastIBI[i] = prevIBI;
                }
            }
        
            sumIBI = prevIBI;
            
            // Shift data over one value
            for(int i = 0; i < 9; i++)
            {
                pastIBI[i] = pastIBI[i+1];
                sumIBI += pastIBI[i];
            }
            
            pastIBI[9] = prevIBI;
            sumIBI = sumIBI/10;
            _bpm = 60/sumIBI;

            // Only report BPM after at least 5 heart beats are registered
            if(beat_count == 5)
            {
                BPM_found = true;
                beat_count = 0;
            }
        }
        
        // If signal is less than threshold, the heart beat is over
        if(((beat < thresh_low) && beat_found) || (currIBI > 2))
        {
            beat_found = false;
            amp = peak - trough;
            thresh_upper = amp/2 + trough;
            peak = thresh_upper;
            trough = thresh_low;
        }
    }
            
    _tmr.stop();

    if(BPM_found) //&& (bpm < BPM_THRESHOLD_MIN || bpm > BPM_THRESHOLD_MAX))
    {
        printf(" BPM = %d", _bpm);
        clpp->addDigitalInput(LPP_DIGITAL_INPUT, this->_bpm);
    }
    else
    {
        printf(" No BPM found");
    }

    printf("\r\n");

    //__enable_irq();

    _tmr.reset();
            
}
