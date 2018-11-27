#pragma once
#include <iostream>
#include <queue>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <eosio/chain/transaction.hpp>
#include <eosio/chain/abi_serializer.hpp>
#include <eosio/chain/abi_def.hpp>

#include <fc/io/json.hpp>
#include <eosio/chain/asset.hpp>
#include "coinxp.match.abi.hpp"
#include "coinxp.bank.abi.hpp"
#include "address.abi.hpp"
#include "coinxp.api.abi.hpp"
#include "coinxp.exchange.abi.hpp"
#include "coinxp.latestprice.abi.hpp"
#include "coinxp.orderdb.abi.hpp"
#include "coinxp.token.abi.hpp"
#include "deposit.tx.abi.hpp"
#include "tokenmap.abi.hpp"
#include "useraddress.abi.hpp"
#include "withdraw.abi.hpp"



typedef boost::function<bool(void)> CxpTask;


template <class T>
class CxpTaskQueue : boost::noncopyable
{
private:
    std::queue<T> m_taskQueue;

public:
    void push_Task(const T& task);

    T pop_Task();

    int get_size();

    void clear();

    void push_head(CxpTaskQueue<T>& q);

    std::queue<T>& get_queue();

};

class CxpThreadPool : public boost::noncopyable
{
public:
    CxpThreadPool()
    {

    }


    ~CxpThreadPool(){

    }

    //停止线程池
    void stop();

    void init(int num);

    void pause();
    void setsuspend(bool suspend){is_suspend=suspend;}
    void resume();
    //添加任务
    void AddNewTask(const CxpTask& task,const fc::variant var,std::string name);

    int get_thread_count();

    uint64_t get_readqueue_size();

    uint64_t get_writequeue_size();

    void setReplay(bool replay);
private:
    //任务队列

    boost::mutex m_mutex;//互斥锁

    boost::mutex m_switch_queue_mutex;

    boost::condition_variable_any m_cond;//条件变量
    CxpTaskQueue<CxpTask> m_taskQueue[2];
    CxpTaskQueue<fc::variant> m_taskvarQueue[2];
    CxpTaskQueue<std::string> m_taskStringQueue[2];

    CxpTaskQueue<CxpTask> m_taskReplayQueue;
    CxpTaskQueue<fc::variant> m_taskReplayvarQueue;
    CxpTaskQueue<std::string> m_taskReplayStringQueue;

    std::atomic<uint32_t> m_read_queue;
    //线程组
    boost::thread_group m_threadGroup;
    int m_threadNum;

    std::atomic<bool> is_run;
    std::atomic<bool> is_suspend;
    std::atomic<uint32_t> m_run_thread;
    std::atomic<bool> is_replay;

    void run();

    void wait();

    uint64_t trxcount;

};


struct CoinPair {
    eosio::chain::symbol_code _currency;
    eosio::chain::symbol_code _commodity;

//    bool operator ==(const CoinPair& coin)
//    {
//        return (coin._currency==_currency && coin._commodity==_commodity);

//    }

};



class CxpTransaction : public boost::noncopyable
{
public:
    CxpTransaction(int num):m_threadNum(num),is_run(false),is_suspend(false),m_run_thread(0),m_read_queue(0),trxcount(0)
    {
        is_replay=false;
    }



    ~CxpTransaction(){

    }



    //停止线程池
    void stop();
    void init();

    void pause();

    void resume();
    //添加任务
    void AddNewTask(const CxpTask& task,const eosio::chain::packed_transaction_ptr& trx);

    int get_thread_count();

    uint64_t get_readqueue_size();

    uint64_t get_writequeue_size();

    void setReplay(bool replay);
    bool getReplay(){return is_replay;}

    eosio::chain::abi_serializer  cxp_match_serializer;
    eosio::chain::abi_serializer  cxp_bank_serializer;
    eosio::chain::abi_serializer  cxp_address_serializer;
    eosio::chain::abi_serializer  cxp_api_serializer;
    eosio::chain::abi_serializer  cxp_exchange_serializer;
    eosio::chain::abi_serializer  cxp_latestprice_serializer;
    eosio::chain::abi_serializer  cxp_orderdb_serializer;
    eosio::chain::abi_serializer  cxp_token_serializer;
    eosio::chain::abi_serializer  cxp_deposit_serializer;
    eosio::chain::abi_serializer  cxp_tokenmap_serializer;
    eosio::chain::abi_serializer  cxp_useraddress_serializer;
    eosio::chain::abi_serializer  cxp_withdraw_serializer;

    fc::microseconds abi_serializer_max_time;

    static CxpTransaction&  instance();
private:
    //任务队列

    boost::mutex m_mutex;//互斥锁

    boost::mutex m_switch_queue_mutex;

    boost::condition_variable_any m_cond;//条件变量
    CxpTaskQueue<CxpTask> m_taskQueue[2];
    CxpTaskQueue<eosio::chain::packed_transaction_ptr> m_transactionptrQueue[2];
    std::atomic<uint32_t> m_read_queue;
    //线程组
    boost::thread_group m_threadGroup;
    int m_threadNum;

    std::atomic<bool> is_run;
    std::atomic<bool> is_suspend;
    std::atomic<uint32_t> m_run_thread;




    void run();

    void wait();
    void dispatch_transaction(const CxpTask& task,const eosio::chain::packed_transaction_ptr& trx);
    uint64_t trxcount;

    CxpThreadPool m_contractPool;

    std::map<CoinPair,std::unique_ptr<CxpThreadPool>> m_match_contractPool;

    std::atomic<bool> is_replay;



};



CxpTransaction& threadpool();

