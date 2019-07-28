

/* -------------------------------- DEBUG FUNCTIONS ---------------------------*/

inline void addLog(int number) {
  char tmpBuffer[40];
  snprintf(tmpBuffer,39,"%d",number);
  addLog(tmpBuffer);
}

inline void addLog(String str) {
  addLog(str.c_str());
}

void addLog(const char *pLog) {
  while (gWeblogCount >= WEBLOG_QUEUE_SIZE) {
    weblogQueueElem_t *pOldLogelem = STAILQ_FIRST(&gLogQueue);
    STAILQ_REMOVE_HEAD(&gLogQueue, logEntry);
    free(pOldLogelem->pLogLine);
    free(pOldLogelem);
    gWeblogCount--;
  }
  weblogQueueElem_t *pNewLogElem = (weblogQueueElem_t *) malloc (sizeof (weblogQueueElem_t));
  char *pLogLine = (char *) malloc (strlen(pLog) + 1);
  strcpy(pLogLine, pLog);
  pNewLogElem->pLogLine = pLogLine;
  STAILQ_INSERT_TAIL(&gLogQueue, pNewLogElem, logEntry);
  gWeblogCount++;
}

void extractLog(String *pOutput,bool isWebOutput) {
  weblogQueueElem_t *pLogElem = NULL;
  STAILQ_FOREACH(pLogElem, &gLogQueue, logEntry) {
     pOutput->concat(pLogElem->pLogLine);
     if (isWebOutput)
       pOutput->concat("<BR>");
     else 
       pOutput->concat("\n");
  }
}
