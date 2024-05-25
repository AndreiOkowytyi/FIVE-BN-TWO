#pragma once

#ifndef FINDTWOVALUESADDUPTONVALUES
#define FINDTWOVALUESADDUPTONVALUES

#include <thread>
#include <algorithm>
#include <vector>
#include <mutex>

const short default_count_th = 4; // Якщо метод hardware_concurrency() == 0 використовуємо default_count_th.

     /*
      FindTwoValuesAddUpToNValuses клас який знаходить два числа массиву, що дають у сумі число value_sum,
      в залежності від кількості елементів в массиві, викоритовується багатопоточний підхід, або
      все виконується в однму потоці.
     */

class FindTwoValuesAddUpToNValuses {
public:
     /*
      Інтерфейсний метод для взаємодії із класом, який повертає результат pair.
      Якщо результат 0, 0 в такому випадку, вектор не містить даних які б нам підходили.
     */

    std::pair<size_t, size_t> findTwoValues(std::vector<size_t> &array_sorted, const size_t value_sum);

private:
    void insertACouple(const size_t first, const size_t second);  // Метод синхронізації данних.
    void initialValue();                                          // Метод який повертає перемінні в почат. положення.

    void searchingForValuesViaThreads(std::vector<size_t>& array_sorted,  // Метод для реалізації багатопоточношо пошуку.
                                      const size_t value_sum, const size_t distance);

    void separateBranchFind(std::vector<size_t>& array_sorted,            // Метод перебору данних в массиві.
                                      const size_t value_find, const size_t distance);

    const short m_c_s_cutoff_value = 1000; // Порогове значення.

    short m_s_start_value = 0;
    short m_s_search_step = 1;

    bool m_b_flag = false;

    std::vector<std::thread> m_v_thread_find_value;

    std::pair<size_t, size_t> m_pair_result;

    std::mutex m_mut;
};

#endif
