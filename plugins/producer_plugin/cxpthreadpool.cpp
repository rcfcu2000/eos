#include "cxpthreadpool.h"
#include <fc/log/logger.hpp>
#include <sys/syscall.h>
#include <appbase/application.hpp>
#include <eosio/chain_plugin/chain_plugin.hpp>
using namespace eosio::chain;
template <class T>
void CxpTaskQueue<T>::push_Task(const T& task)
{
     m_taskQueue.push(task);
}
template <class T>
T CxpTaskQueue<T>::pop_Task()
{
    //指向队列首部
    T task(m_taskQueue.front());
    //出队列
    m_taskQueue.pop();
    return task;
}
template <class T>
int CxpTaskQueue<T>::get_size()
{
     return m_taskQueue.size();
}

template <class T>
void CxpTaskQueue<T>::clear()
{
    std::queue<T> empty;
    std::swap( m_taskQueue, empty );
}
template <class T>
std::queue<T>& CxpTaskQueue<T>::get_queue()
{
    return m_taskQueue;
}
template <class T>
void CxpTaskQueue<T>::push_head(CxpTaskQueue<T>& q)
{
    std::queue<T> temp;
    std::swap( m_taskQueue, temp );
    std::swap( m_taskQueue, q.get_queue() );
    while(temp.size()>0)
    {
        m_taskQueue.push(temp.front());
        temp.pop();
    }

}

void CxpThreadPool::run()
{

    while(is_run)
    {
        //一直处理线程池的任务
        //加上互斥锁
        {

            //ilog("run1 m_read_queue:${a} size:${b} write_queue:${c}",("a",m_read_queue)("b",m_taskQueue[m_read_queue].get_size())("c",m_taskQueue[(m_read_queue+1)%2].get_size()));
            boost::unique_lock<boost::mutex> lock(m_mutex);
            //如果队列中没有任务，则等待互斥锁
            if(m_taskQueue[0].get_size()==0 && m_taskQueue[1].get_size()==0)
            {
                m_cond.wait(lock);
            }

            if(is_suspend)
            {

                m_cond.wait(lock);
            }

            //ilog("run2 m_read_queue:${a} size:${b} write_queue:${c}",("a",m_read_queue)("b",m_taskQueue[m_read_queue].get_size())("c",m_taskQueue[(m_read_queue+1)%2].get_size()));
            //如果读取队列中没有任务，则切换队列
            if(m_taskQueue[m_read_queue].get_size()==0)
            {
                boost::unique_lock<boost::mutex> lock(m_switch_queue_mutex);
                m_read_queue=(++m_read_queue)%2;
            }

           //ilog("run3 m_read_queue:${a} size:${b} write_queue:${c}",("a",m_read_queue)("b",m_taskQueue[m_read_queue].get_size())("c",m_taskQueue[(m_read_queue+1)%2].get_size()));
           if(m_taskQueue[m_read_queue].get_size()==0)
               continue;


                CxpTask task = m_taskQueue[m_read_queue].pop_Task();
                fc::variant var = m_taskvarQueue[m_read_queue].pop_Task();
                string name = m_taskStringQueue[m_read_queue].pop_Task();



                elog("${a} ${b} tid=${c}",("a",name)("b",var)("c",syscall(SYS_gettid)));
                m_run_thread++;

                lock.unlock();
                task();
//                if(task())
//                {
//                    m_taskReplayQueue.push_Task(task);
//                    m_taskReplayvarQueue.push_Task(var);
//                    m_taskReplayStringQueue.push_Task(name);
//                }

                m_run_thread--;


        }

    }
}

void CxpThreadPool::wait()
{
    m_threadGroup.join_all();//等待线程池处理完成！
}

void CxpThreadPool::stop()
{
    is_run = false;
    is_suspend=false;
    m_cond.notify_all();
    wait();
}

void CxpThreadPool::init(int num)
{
    m_threadNum=num,is_run=false,is_suspend=true,m_run_thread=0,m_read_queue=0,trxcount=0;
    is_replay=false;
    if(m_threadNum <= 0) return;
    is_run = true;
    for (int i=0;i<m_threadNum;i++)
    {
        //生成多个线程，绑定run函数，添加到线程组
        m_threadGroup.add_thread(
            new boost::thread(boost::bind(&CxpThreadPool::run,this)));
    }
}

void CxpThreadPool::pause()
{

    //ilog("CxpThreadPool push trxcount:${a} m_threadNum=${b}",("a",trxcount)("b",m_threadNum));
    trxcount=0;
    is_suspend=true;
    boost::unique_lock<boost::mutex> lock(m_mutex);


    while(m_run_thread>0)
    {

    }

}

void CxpThreadPool::resume()
{
    is_suspend=false;

//    if(!is_replay)
//    {
//        m_taskReplayQueue.clear();
//        m_taskReplayvarQueue.clear();
//        m_taskReplayStringQueue.clear();
//    }
//    else
//    {
//        if(m_taskReplayQueue.get_size()>0)
//        {
//            elog("replay trx ${a}",("a",m_taskReplayQueue.get_size()));
//            m_taskQueue[m_read_queue].push_head(m_taskReplayQueue);
//            m_taskvarQueue[m_read_queue].push_head(m_taskReplayvarQueue);
//            m_taskStringQueue[m_read_queue].push_head(m_taskReplayStringQueue);
//        }

//        is_replay=false;
//    }


    if(m_taskQueue[0].get_size()>0 || m_taskQueue[1].get_size()>0)
        m_cond.notify_all();

}

//添加任务
void CxpThreadPool::AddNewTask(const CxpTask& task,const fc::variant var,std::string name)
{

    if(m_taskQueue[(m_read_queue+1)%2].get_size()>=100000)
        return ;
    boost::unique_lock<boost::mutex> lock(m_switch_queue_mutex);
    m_taskQueue[(m_read_queue+1)%2].push_Task(task);
    m_taskvarQueue[(m_read_queue+1)%2].push_Task(var);
    m_taskStringQueue[(m_read_queue+1)%2].push_Task(name);
    trxcount++;
    lock.unlock();

    if(!is_suspend&& is_run)
        m_cond.notify_one();
}

int CxpThreadPool::get_thread_count(){return m_threadNum;}

uint64_t CxpThreadPool::get_readqueue_size()
{
    return m_taskQueue[m_read_queue].get_size();
}


uint64_t CxpThreadPool::get_writequeue_size()
{
    return m_taskQueue[(m_read_queue+1)%2].get_size();
}

void CxpThreadPool::setReplay(bool replay)
{
    is_replay=replay;
}


//-------------------------------------------------------------------------------------------------
bool operator < (const CoinPair& left, const CoinPair& right)
{

    if (left._currency < right._currency) // 主key
    {
        return true; // 主key小，就小
    }

    if (left._currency > right._currency) // 主key
    {
        return false; // 主key大，就大
    }

    return left._commodity < right._commodity; // 主key相等，再比较次key
}
void CxpTransaction::run()
{

    while(is_run)
    {
        //一直处理线程池的任务
        //加上互斥锁
        {

            //ilog("run1 m_read_queue:${a} size:${b} write_queue:${c}",("a",m_read_queue)("b",m_taskQueue[m_read_queue].get_size())("c",m_taskQueue[(m_read_queue+1)%2].get_size()));
            boost::unique_lock<boost::mutex> lock(m_mutex);
            //如果队列中没有任务，则等待互斥锁
            if(m_taskQueue[0].get_size()==0 && m_taskQueue[1].get_size()==0)
            {
                m_cond.wait(lock);
            }

            if(is_suspend)
            {

                m_cond.wait(lock);
            }

            //ilog("run2 m_read_queue:${a} size:${b} write_queue:${c}",("a",m_read_queue)("b",m_taskQueue[m_read_queue].get_size())("c",m_taskQueue[(m_read_queue+1)%2].get_size()));
            //如果读取队列中没有任务，则切换队列
            if(m_taskQueue[m_read_queue].get_size()==0)
            {
                boost::unique_lock<boost::mutex> lock(m_switch_queue_mutex);
                m_read_queue=(++m_read_queue)%2;
            }

           //ilog("run3 m_read_queue:${a} size:${b} write_queue:${c}",("a",m_read_queue)("b",m_taskQueue[m_read_queue].get_size())("c",m_taskQueue[(m_read_queue+1)%2].get_size()));
           if(m_taskQueue[m_read_queue].get_size()==0)
               continue;

                CxpTask task = m_taskQueue[m_read_queue].pop_Task();
                packed_transaction_ptr trx = m_transactionptrQueue[m_read_queue].pop_Task();
                m_run_thread++;
                lock.unlock();
                dispatch_transaction(task,trx);
                m_run_thread--;


        }

    }
}

void CxpTransaction::dispatch_transaction(const CxpTask& task,const eosio::chain::packed_transaction_ptr& trx)
{
    eosio::chain::transaction t=trx->get_transaction();
    if(t.actions.size()>0)
    {
        eosio::chain::name act_account=t.actions[0].account;
        std::string account_str=act_account.to_string();
        eosio::chain::name act_name=t.actions[0].name;
        std::string name_str=act_name.to_string();
        std::string demo=account_str+" ";
        demo+=name_str;
        demo+=" ";
       // elog( "account=${a} name=${b}",("a",account_str)("b",name_str));
        if(account_str=="cxp.match" && name_str=="ask")
        {
            auto v=cxp_match_serializer.binary_to_variant(cxp_match_serializer.get_action_type(act_name),t.actions[0].data,abi_serializer_max_time);
            std::string currency1=v["currency"].get_string();
            std::string commodity1=v["commodity"].get_string();
            //size_t next=currency.find(' ');
            eosio::chain::asset currency=asset::from_string(v["currency"].get_string());
            eosio::chain::asset commodity=asset::from_string(v["commodity"].get_string());
            const CoinPair pair={currency.get_symbol().to_symbol_code(),commodity.get_symbol().to_symbol_code()};
            std::map<CoinPair,std::unique_ptr<CxpThreadPool>>::iterator it=m_match_contractPool.find(pair);
            if(it==m_match_contractPool.end())
            {
                //CxpThreadPool pool;
               // std::unique_ptr<CxpThreadPool> pool=std::make_unique<CxpThreadPool>();
                //pool->init(1);

                it = m_match_contractPool.emplace(pair,std::make_unique<CxpThreadPool>()).first;
                it->second->init(1);
            }
            it->second->AddNewTask(task,v,demo);
            //elog( "vvvv account=${a} name=${b} v=${c}",("a",account_str)("b",name_str)("c",v));

        }
        else if(account_str=="coinxp.bank")
        {
            auto v=cxp_bank_serializer.binary_to_variant(cxp_bank_serializer.get_action_type(act_name),t.actions[0].data,abi_serializer_max_time);
            m_contractPool.AddNewTask(task,v,demo);
        }
        else
            m_contractPool.AddNewTask(task,fc::variant(),demo);


    }
}

void CxpTransaction::wait()
{
    m_threadGroup.join_all();//等待线程池处理完成！
}

void CxpTransaction::stop()
{
    is_run = false;
    is_suspend=false;
    m_cond.notify_all();
    m_contractPool.stop();
    std::map<CoinPair,std::unique_ptr<CxpThreadPool>>::iterator it=m_match_contractPool.begin();
    for(;it!=m_match_contractPool.end();it++)
    {
        it->second->stop();
    }
    wait();
}

void CxpTransaction::init()
{
    if(m_threadNum <= 0) return;
    m_contractPool.init(1);
    is_run = true;
    for (int i=0;i<m_threadNum;i++)
    {
        //生成多个线程，绑定run函数，添加到线程组
        m_threadGroup.add_thread(
            new boost::thread(boost::bind(&CxpTransaction::run,this)));
    }

    abi_serializer_max_time = appbase::app().get_plugin<eosio::chain_plugin>().get_abi_serializer_max_time();
    cxp_match_serializer.set_abi(fc::json::from_string(coinxp_match_abi).as<eosio::abi_def>(), abi_serializer_max_time);
    cxp_bank_serializer.set_abi(fc::json::from_string(coinxp_bank_abi).as<eosio::abi_def>(), abi_serializer_max_time);
    cxp_address_serializer.set_abi(fc::json::from_string(address_abi).as<eosio::chain::abi_def>(), abi_serializer_max_time);;
    cxp_api_serializer.set_abi(fc::json::from_string(coinxp_api_abi).as<eosio::chain::abi_def>(), abi_serializer_max_time);;
    cxp_exchange_serializer.set_abi(fc::json::from_string(coinxp_exchange_abi).as<eosio::chain::abi_def>(), abi_serializer_max_time);;
    cxp_latestprice_serializer.set_abi(fc::json::from_string(coinxp_latestprice_abi).as<eosio::chain::abi_def>(), abi_serializer_max_time);;
    cxp_orderdb_serializer.set_abi(fc::json::from_string(coinxp_orderdb_abi).as<eosio::chain::abi_def>(), abi_serializer_max_time);;
    cxp_token_serializer.set_abi(fc::json::from_string(coinxp_token_abi).as<eosio::chain::abi_def>(), abi_serializer_max_time);;
    cxp_deposit_serializer.set_abi(fc::json::from_string(deposit_tx_abi).as<eosio::chain::abi_def>(), abi_serializer_max_time);;
    cxp_tokenmap_serializer.set_abi(fc::json::from_string(tokenmap_abi).as<eosio::chain::abi_def>(), abi_serializer_max_time);;
    cxp_useraddress_serializer.set_abi(fc::json::from_string(useraddress_abi).as<eosio::chain::abi_def>(), abi_serializer_max_time);;
    cxp_withdraw_serializer.set_abi(fc::json::from_string(withdraw_abi).as<eosio::chain::abi_def>(), abi_serializer_max_time);;






}

void CxpTransaction::pause()
{

    //ilog("CxpTransaction push trxcount:${a}",("a",trxcount));
    trxcount=0;


    m_contractPool.setsuspend(true);
    std::map<CoinPair,std::unique_ptr<CxpThreadPool>>::iterator it=m_match_contractPool.begin();
    for(;it!=m_match_contractPool.end();it++)
    {
        it->second->setsuspend(true);
    }

    m_contractPool.pause();
    it=m_match_contractPool.begin();
    for(;it!=m_match_contractPool.end();it++)
    {
        it->second->pause();
    }


//    is_suspend=true;
//    boost::unique_lock<boost::mutex> lock(m_mutex);


//    while(m_run_thread>0)
//    {

//    }

}

void CxpTransaction::resume()
{
    is_replay=false;
    m_contractPool.resume();
    std::map<CoinPair,std::unique_ptr<CxpThreadPool>>::iterator it=m_match_contractPool.begin();
    for(;it!=m_match_contractPool.end();it++)
    {
        it->second->resume();
    }

//    is_suspend=false;

//    if(m_taskQueue[0].get_size()>0 || m_taskQueue[1].get_size()>0)
//        m_cond.notify_all();
}

//添加任务
void CxpTransaction::AddNewTask(const CxpTask& task,const eosio::chain::packed_transaction_ptr& trx)
{

    if(m_taskQueue[(m_read_queue+1)%2].get_size()>=100000)
        return ;

    boost::unique_lock<boost::mutex> lock(m_switch_queue_mutex);
    m_taskQueue[(m_read_queue+1)%2].push_Task(task);
    m_transactionptrQueue[(m_read_queue+1)%2].push_Task(trx);
    trxcount++;
    lock.unlock();

    if(!is_suspend&& is_run)
        m_cond.notify_one();
}

int CxpTransaction::get_thread_count(){return m_threadNum;}

uint64_t CxpTransaction::get_readqueue_size()
{
    return m_taskQueue[m_read_queue].get_size();
}


uint64_t CxpTransaction::get_writequeue_size()
{
    return m_taskQueue[(m_read_queue+1)%2].get_size();
}

void CxpTransaction::setReplay(bool replay)
{
    if(is_replay==replay)
        return ;
    is_replay=replay;
    m_contractPool.setReplay(replay);
    std::map<CoinPair,std::unique_ptr<CxpThreadPool>>::iterator it=m_match_contractPool.begin();
    it=m_match_contractPool.begin();
    for(;it!=m_match_contractPool.end();it++)
    {
        it->second->setReplay(replay);
    }
}

CxpTransaction& CxpTransaction::instance() {
   static CxpTransaction _threadpool(1);
   return _threadpool;
}

CxpTransaction& threadpool() { return CxpTransaction::instance(); }
