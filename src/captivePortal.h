#ifndef _captivePortal_h
#define _captivePortal_h

#pragma once

class CaptivePortalManager
{
  public:
    static bool captivePortalCalled;

    CaptivePortalManager(WiFiManager *wifiManager);
    ~CaptivePortalManager();
    static void captivePortalManagerCallback(WiFiManager *wifiManager);

  private:
    WiFiManager *wifiManager = NULL;
};


CaptivePortalManager::CaptivePortalManager(WiFiManager *wifiManager)
{
  this->wifiManager = wifiManager;

  // добавление пользовательских параметров: id, placeholder, prompt/default value, length[, custom html]
  // ...

  if (wifiManager != NULL)
  {
    wifiManager->setAPCallback(CaptivePortalManager::captivePortalManagerCallback);
  }
}

CaptivePortalManager::~CaptivePortalManager()
{
  this->wifiManager = NULL;
}

void CaptivePortalManager::captivePortalManagerCallback(WiFiManager *wifiManager)
{
  CaptivePortalManager::captivePortalCalled = true;
  // сохранение введённых значений пользовательских параметров
  // ...
}

bool CaptivePortalManager::captivePortalCalled = false;

#endif
