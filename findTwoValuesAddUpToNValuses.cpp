#include "FindTwoValuesAddUpToNValuses.h"

std::pair<size_t, size_t> FindTwoValuesAddUpToNValuses::findTwoValues(std::vector<size_t>& array_sorted, const size_t value_sum) {

    if (array_sorted.size() < 2 || !std::is_sorted(array_sorted.begin(), array_sorted.end())) return this->m_pair_result;

    auto lower_bound_iter = std::lower_bound(array_sorted.begin(), array_sorted.end(), value_sum);

    /*
     Перевірка умов. Якщо кількість елементів які треба перебрати більша за m_c_s_cutoff_value
     використовуємо багатопоточність. Якщо ні, перебираємо елементи в одному потоку.
    */

    if (std::distance(array_sorted.begin(), lower_bound_iter) > this->m_c_s_cutoff_value) {

        if (lower_bound_iter != array_sorted.end()) 
            
                  searchingForValuesViaThreads(array_sorted, value_sum, std::distance(array_sorted.begin(), lower_bound_iter));

        else searchingForValuesViaThreads(array_sorted, value_sum, array_sorted.size());

    } else {

        if (lower_bound_iter != array_sorted.end()) 

                      this->separateBranchFind(array_sorted, value_sum, std::distance(array_sorted.begin(), lower_bound_iter));

        else this->separateBranchFind(array_sorted, value_sum, array_sorted.size());
    }

    std::pair<size_t, size_t> temp_pair(this->m_pair_result.first, this->m_pair_result.second);

    // Повертаємо дані в початкове положення, для подальшого використання класу за допомогою initialValue.

    this->initialValue();

    return temp_pair;
}

    /*
     Метод insertACouple для усунення запису одразу кількома потоками в m_pair_result. Додаткова перевірка
     m_flag для усунення повторонго запису даних до m_pair_result.
    */

void FindTwoValuesAddUpToNValuses::insertACouple(const size_t first, const size_t second) {

    std::unique_lock<std::mutex> uniq_lock(this->m_mut);

    if (!this->m_b_flag) {

        this->m_pair_result.first  = first;
        this->m_pair_result.second = second;
        this->m_b_flag = true;
    }
}


void FindTwoValuesAddUpToNValuses::initialValue() {

    this->m_b_flag        = false;
    this->m_s_start_value = 0;
    this->m_s_search_step = 1;

    this->m_pair_result.first  = 0;
    this->m_pair_result.second = 0;
}

    /*
     Метод searchingForValuesViaThreads створює группу потоків, які шукають два числа що в сумі відповідають value_sum.
    */

void FindTwoValuesAddUpToNValuses::searchingForValuesViaThreads(std::vector<size_t>& array_sorted,
                                                                const size_t value_sum, const size_t distance) {

    const short count_hardware_concurrency = std::thread::hardware_concurrency();

    auto forThread = [&](const short count_thread) {

        this ->m_s_search_step = count_thread;

        for (short x = 0; x < count_thread; x++)

            this->m_v_thread_find_value.push_back(std::thread([&]() {

                this->separateBranchFind(array_sorted, value_sum, distance);
            }));
    };

    /*
     Якщо count_thread == 0 використоавуємо default_count_th (4), за допомогою якого створюємо 4 потоки.

     Якщо distance, тобто кількість данних які треба перебрати менша за default_count_th, або за count_thread, в такому
     випадку до lambda forThread передається значення distance, яке буде відповідати кількості створенних потоків.
    */

    if (count_hardware_concurrency == 0) {

        if(distance < default_count_th) forThread(distance);
        else forThread(default_count_th);

    } else {

        if (distance < count_hardware_concurrency) forThread(distance);
        else forThread(count_hardware_concurrency);
    }

    for (auto &it : this->m_v_thread_find_value) it.join();
}

   /*
     Метод separateBranchFind перебирає вектор, та шукає два значення сумма яких відповідає value_find.
     Метод використовує стартову перемінну m_s_start_value для початку перебору массиву,
     та після закінчення порівняння, переходить на інше значення за допомогою m_s_search_step.
   */

void FindTwoValuesAddUpToNValuses::separateBranchFind(std::vector<size_t>& array_sorted,
                                                      const size_t value_find, const size_t distance) {

    size_t result_sum = 0;
    size_t temp_index = this->m_s_start_value;

    this->m_s_start_value++;

    while (true) {

        if (distance > temp_index) { // distance - кількість єлементів в рамках яких ми шукаємо 2 значення.

            for (size_t x = 0; x < distance; x++) {

                if (temp_index == x) continue;

                if (this->m_b_flag) return; // Якщо значення вже найдені, виходимо із функції.

                result_sum = array_sorted[temp_index] + array_sorted[x];

                if (result_sum > value_find) break; // Якщо result_sum більше value_find виходимо із for.

                if (result_sum == value_find) this->insertACouple(array_sorted[temp_index], array_sorted[x]);
            }

        } else return;

        temp_index += this->m_s_search_step;
    }
}
