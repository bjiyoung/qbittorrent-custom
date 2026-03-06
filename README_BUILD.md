# qBittorrent 5.1.4 Custom Build

## 포함된 커스텀 패치

1. **다운로드 완료 후 상태 자동 변경**
   - 100% 다운로드 + 완료 폴더 이동이 끝나면 토렌트 상태가 자동으로 "완료됨"으로 전환

2. **파일 추가 다이얼로그에 크기 기준 선택 기능**
   - `Select All` / `Select None` 버튼 옆에 최소 크기 필터 추가
   - 체크박스로 활성화, 크기 입력 + 단위(KB/MB/GB/TB) 선택
   - 설정값은 앱 재시작 후에도 유지됨

---

## GitHub Actions로 exe 빌드하기

### 사전 요구사항
- [Git](https://git-scm.com/download/win)
- [GitHub CLI (gh)](https://cli.github.com) — 없으면 스크립트가 자동 설치

### 방법 1: 스크립트 사용 (권장)

```
1. upload_and_build.bat  실행  →  GitHub 업로드 + 빌드 시작 (약 30~50분)
2. download.bat          실행  →  완성된 exe 자동 다운로드
```

### 방법 2: 콘솔 직접 입력

```bat
:: 소스 루트로 이동
cd qbittorrent-5.1.4

:: GitHub 로그인
gh auth login

:: 저장소 생성 및 업로드
git init
git add .
git commit -m "qBittorrent 5.1.4 custom"
gh repo create qbittorrent-custom --public --source=. --push

:: 빌드 시작
gh workflow run build-release.yml

:: 상태 확인
gh run list --workflow=build-release.yml

:: 다운로드 (RUN_ID는 위 명령어 결과에서 확인)
gh run download RUN_ID --name qBittorrent-5.1.4-custom-windows-x64 --dir output
```

### 빌드 결과물
`output/qBittorrent/` 폴더에 실행에 필요한 모든 DLL과 `qbittorrent.exe` 가 들어있습니다.
별도 설치 없이 폴더째로 사용 가능합니다.
