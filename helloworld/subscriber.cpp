/*
 * Copyright(c) 2006 to 2021 ZettaScale Technology and others
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License v. 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0, or the Eclipse Distribution License
 * v. 1.0 which is available at
 * http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * SPDX-License-Identifier: EPL-2.0 OR BSD-3-Clause
 */
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <thread>

/* Include the C++ DDS API. */
#include "dds/dds.hpp"

/* Include data type and specific traits to be used with the C++ DDS API. */
#include "HelloWorldData.hpp"

using namespace org::eclipse::cyclonedds;
using namespace std;

class Publisher
{
public:
    void init();
    int publish();

private:
    // std::shared_ptr<dds::domain::DomainParticipant> participant_;
    // shared_ptr<dds::topic::Topic<HelloWorldData::Msg>> topic_;
    // shared_ptr<dds::sub::Subscriber> subscriber_;
    shared_ptr<dds::sub::DataReader<HelloWorldData::Msg>> reader_;
};

void Publisher::init()
{

    std::cout << "=== [Subscriber] Create reader." << std::endl;

    // participant_ = make_shared<dds::domain::DomainParticipant>(dds::domain::DomainParticipant(domain::default_id()));
    dds::domain::DomainParticipant participant(domain::default_id());
    // topic_ =  make_shared<dds::topic::Topic<HelloWorldData::Msg>>(dds::topic::Topic<HelloWorldData::Msg>(*participant_, "HelloWorldData_Msg"));
    dds::topic::Topic<HelloWorldData::Msg> topic(participant, "HelloWorldData_Msg");
    // subscriber_ = make_shared<dds::sub::Subscriber>(dds::sub::Subscriber(*participant_));
    dds::sub::Subscriber subscriber(participant);

    reader_ = make_shared<dds::sub::DataReader<HelloWorldData::Msg>>(dds::sub::DataReader<HelloWorldData::Msg>(subscriber, topic));
    // dds::sub::DataReader<HelloWorldData::Msg> reader(subscriber, topic);
}

int Publisher::publish()
{
    try
    {

        /* Now, the reader can be created to subscribe to a HelloWorld message. */
        auto reader = *reader_;

        /* Poll until a message has been read.
         * It isn't really recommended to do this kind wait in a polling loop.
         * It's done here just to illustrate the easiest way to get data.
         * Please take a look at Listeners and WaitSets for much better
         * solutions, albeit somewhat more elaborate ones. */
        std::cout << "=== [Subscriber] Wait for message." << std::endl;
        bool poll = true;
        while (poll)
        {
            /* For this example, the reader will return a set of messages (aka
             * Samples). There are other ways of getting samples from reader.
             * See the various read() and take() functions that are present. */
            dds::sub::LoanedSamples<HelloWorldData::Msg> samples;

            /* Try taking samples from the reader. */
            samples = reader.take();

            /* Are samples read? */
            if (samples.length() > 0)
            {
                /* Use an iterator to run over the set of samples. */
                dds::sub::LoanedSamples<HelloWorldData::Msg>::const_iterator sample_iter;
                for (sample_iter = samples.begin();
                     sample_iter < samples.end();
                     ++sample_iter)
                {
                    /* Get the message and sample information. */
                    const HelloWorldData::Msg &msg = sample_iter->data();
                    const dds::sub::SampleInfo &info = sample_iter->info();

                    /* Sometimes a sample is read, only to indicate a data
                     * state change (which can be found in the info). If
                     * that's the case, only the key value of the sample
                     * is set. The other data parts are not.
                     * Check if this sample has valid data. */
                    if (info.valid())
                    {
                        std::cout << "=== [Subscriber] Message received:" << std::endl;
                        std::cout << "    userID  : " << msg.userID() << std::endl;
                        std::cout << "    Message : \"" << msg.message() << "\"" << std::endl;

                        /* Only 1 message is expected in this example. */
                        poll = false;
                    }
                }
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }
        }
    }
    catch (const dds::core::Exception &e)
    {
        std::cerr << "=== [Subscriber] DDS exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (const std::exception &e)
    {
        std::cerr << "=== [Subscriber] C++ exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "=== [Subscriber] Done." << std::endl;

    return EXIT_SUCCESS;
}

int main()
{
    Publisher mypub;
    mypub.init();
    mypub.publish();
    return 0;
}