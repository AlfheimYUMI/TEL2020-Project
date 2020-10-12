menu = {
    '執行主程式': {
        '返回': None,
        '自動': 'run_auto',
        '手動': 'run_remote',
    },
    '更新資料': 'fun_reflashGIT',
    '程式相關': {
        '返回': None,
        # 自動產生清單
        '執行中程式': {
            '返回': None,
        },
        '執行其他程式': {
            '返回': None,
        }
    },
    '網路相關': {
        '返回': None,
        'IP': 'fun_getIP',
        # 連接其他WIFI
    },
    '系統': {
        '返回': None,
        '重新啟動': 'sudo reboot now',
        '關機': 'sudo shutdown now',
        '結束': None
    },
}